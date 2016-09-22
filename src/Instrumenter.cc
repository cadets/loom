//! @file Instrumenter.cc  Definition of @ref loom::Instrumenter.
/*
 * Copyright (c) 2016 Jonathan Anderson
 * All rights reserved.
 *
 * This software was developed by BAE Systems, the University of Cambridge
 * Computer Laboratory, and Memorial University under DARPA/AFRL contract
 * FA8650-15-C-7558 ("CADETS"), as part of the DARPA Transparent Computing
 * (TC) research program.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "Instrumenter.hh"
#include "Logger.hh"

#include <llvm/IR/Module.h>

#include <sstream>

using namespace llvm;
using namespace loom;
using namespace std;


unique_ptr<Instrumenter>
Instrumenter::Create(Module& Mod, NameFn NF, unique_ptr<InstrStrategy> S)
{
  return unique_ptr<Instrumenter>(new Instrumenter(Mod, NF, std::move(S)));
}


Instrumenter::Instrumenter(llvm::Module& Mod, NameFn NF,
                           unique_ptr<InstrStrategy> S)
  : Mod(Mod), Strategy(std::move(S)), Name(NF)
{
}


bool Instrumenter::Instrument(llvm::Instruction *I)
{
  // If this instruction terminates a block, we need to treat it a bit
  // differently, placing instrumentation before it rather than after.
  const bool Terminator = I->isTerminator();

  // If this instruction is a terminator *or* is of void type, it doesn't
  // have a value for us to log.
  const bool Void = Terminator or I->getType()->isVoidTy();

  auto *OpcodeTy = IntegerType::get(Mod.getContext(), 32);
  unsigned Opcode = I->getOpcode();

  ParamVec ValueDescriptions = { { "opcode", OpcodeTy } };
  vector<Value*> Values = { ConstantInt::get(OpcodeTy, Opcode) };

  if (not Void)
  {
    ValueDescriptions.emplace_back(I->getName(), I->getType());
    Values.push_back(I);
  }

  for (Use& U : I->operands()) {
    Value *V = U.get();

    // Don't use the address of an LLVM intrinsic: report its name instead.
    if (Function *F = dyn_cast<Function>(V)) {
      if (F->hasLLVMReservedName()) {
        V = IRBuilder<>(I).CreateGlobalStringPtr(F->getName(), "fn_name");
      }
    }

    Type *T = V->getType();
    if (T->isMetadataTy()) {
      // Ignore metadata-consuming instructions such as calls to
      // @llvm.dbg.declare().
      return false;
    }

    ValueDescriptions.emplace_back(V->getName(), T);
    Values.push_back(V);
  }

  // We don't handle varargs generically: that needs to be done by
  // function- or call-specific instrumentation.
  constexpr bool Varargs = false;

  // Instrument all non-terminators after the instructor, so that we can
  // capture the instruction's value (if non-void).
  const bool AfterInst = not Terminator;

  Strategy->Instrument(I, "instruction", "instruction",
                       ValueDescriptions, Values, Varargs, AfterInst);

  return true;
}

bool Instrumenter::Instrument(CallInst *Call, const Policy::Directions& D)
{
  bool ModifiedIR = false;

  for(auto Dir : D) {
    ModifiedIR |= Instrument(Call, Dir);
  }

  return ModifiedIR;
}


bool Instrumenter::Instrument(llvm::CallInst *Call, Policy::Direction Dir)
{
  Function* Target = Call->getCalledFunction();
  assert(Target); // TODO: support indirect targets, too

  // Get some relevant details about the call and the target function.
  const string TargetName = Target->getName();
  const bool VarArgs = Target->isVarArg();
  Type *CallType = Call->getType();
  const bool voidFunction = CallType->isVoidTy();
  const bool Return = (Dir == Policy::Direction::Out);

  const string Description = Return ? "return" : "call";
  const string FormatStringPrefix = Description + " " + TargetName + ":";
  const string InstrName = Name({ Description, TargetName });

  // Start by copying static and dynamic value details from the target function.
  ParamVec Parameters = GetParameters(Target);
  vector<Value*> Arguments(Call->getNumArgOperands());
  std::copy(Call->arg_begin(), Call->arg_end(), Arguments.begin());

  // The return value, if present, comes first in the instrumentation.
  if (Return and not voidFunction) {
    Parameters.emplace(Parameters.begin(), "retval", Call->getType());
    Arguments.emplace(Arguments.begin(), Call);
  }

  bool InstrAfterCall = Return;
  Strategy->Instrument(Call, InstrName, FormatStringPrefix,
                       Parameters, Arguments, VarArgs, InstrAfterCall);

  return true;
}


bool
Instrumenter::Instrument(Function& Fn, const Policy::Directions& D) {
  bool ModifiedIR = false;

  for (auto Dir : D) {
    ModifiedIR |= Instrument(Fn, Dir);
  }

  return ModifiedIR;
}


bool
Instrumenter::Instrument(Function& Fn, Policy::Direction Dir) {
  const bool Return = (Dir == Policy::Direction::Out);
  const string Description = Return ? "leave" : "enter";
  StringRef FnName = Fn.getName();

  assert(isa<PointerType>(Fn.getType()));
  FunctionType *FnType = dyn_cast<FunctionType>(Fn.getType()->getElementType());
  assert(FnType);
  const bool voidFunction = FnType->getReturnType()->isVoidTy();

  vector<Value*> Arguments;
  vector<Parameter> InstrParameters;

  if (Return and not voidFunction) {
    Arguments.push_back(nullptr);
    InstrParameters.emplace_back("retval", FnType->getReturnType());
  }

  for (auto& Arg : Fn.getArgumentList()) {
    Arguments.push_back(&Arg);
    InstrParameters.emplace_back(Arg.getName(), Arg.getType());
  }

  const string InstrName = Name({ Description, FnName });
  string FormatStringPrefix = (Description + " " + FnName + ":").str();

  if (Return) {
    // Instrument all returns from the function:
    SmallVector<ReturnInst*, 4> Returns;

    for (auto& Block : Fn) {
      TerminatorInst *Terminator = Block.getTerminator();
      if (auto *Ret = dyn_cast<ReturnInst>(Terminator)) {
        Returns.push_back(Ret);
      }
    }

    for (ReturnInst *Ret : Returns) {
      // This is a return: pass the return value and then the function's
      // parameters to the instrumentation.
      if (not voidFunction) {
        Arguments[0] = Ret->getReturnValue();
      }

      Strategy->Instrument(Ret, InstrName, FormatStringPrefix,
                           InstrParameters, Arguments);
    }

  } else {
    // Instrument the function's preamble:
    assert(not Fn.getBasicBlockList().empty());
    BasicBlock& Entry = Fn.getBasicBlockList().front();

    Strategy->Instrument(&Entry.front(), InstrName, FormatStringPrefix,
                         InstrParameters, Arguments);
  }

  return false;
}


bool Instrumenter::Instrument(GetElementPtrInst *GEP, LoadInst *Load,
                              StringRef FieldName) {
  StructType *SourceType = dyn_cast<StructType>(GEP->getSourceElementType());
  assert(SourceType);
  assert(SourceType->getName().startswith("struct."));
  const string StructName = SourceType->getName().substr(7);

  ParamVec Parameters {
    { "source", GEP->getPointerOperandType() },
    { "value", Load->getType() },
  };

  vector<Value*> Arguments {
    GEP->getPointerOperand(),
    Load,
  };

  const string InstrName = Name(
    { "load", "struct", StructName, "field", FieldName }
  );

  const string FormatStringPrefix =
    (StructName + "." + FieldName + " load:").str();

  Strategy->Instrument(Load, InstrName, FormatStringPrefix,
                       Parameters, Arguments, false, true);

  return true;
}


bool Instrumenter::Instrument(GetElementPtrInst *GEP, StoreInst *Store,
                              StringRef FieldName) {
  StructType *SourceType = dyn_cast<StructType>(GEP->getSourceElementType());
  assert(SourceType);
  assert(SourceType->getName().startswith("struct."));
  const string StructName = SourceType->getName().substr(7);

  ParamVec Parameters {
    { "source", GEP->getPointerOperandType() },
    { "value", Store->getValueOperand()->getType() },
  };

  vector<Value*> Arguments {
    GEP->getPointerOperand(),
    Store->getValueOperand(),
  };

  const string InstrName = Name(
    { "store", "struct", StructName, "field", FieldName }
  );

  const string FormatStringPrefix =
    (StructName + "." + FieldName + " store:").str();

  Strategy->Instrument(Store, InstrName, FormatStringPrefix,
                       Parameters, Arguments);

  return true;
}


uint32_t Instrumenter::FieldNumber(GetElementPtrInst *GEP) {
  // A field access getelementptr should have two indices:
  // a zero and the target field number (a constant integer).
  Use *IdxZero = GEP->idx_begin();
  Use *FieldIdx = IdxZero + 1;
  assert(GEP->getNumIndices() == 2);
  assert(dyn_cast<ConstantInt>(IdxZero->get())->isZero());
  assert(isa<ConstantInt>(FieldIdx->get()));

  ConstantInt *FieldNum = dyn_cast<ConstantInt>(FieldIdx->get());
  return FieldNum->getZExtValue();
}
