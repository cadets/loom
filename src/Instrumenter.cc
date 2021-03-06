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
 * Copyright (c) 2018 Stephen Lee
 * All rights reserved.
 *
 * This software was developed by SRI International, Purdue University,
 * University of Wisconsin and University of Georgia  under DARPA/AFRL
 * contract FA8650-15-C-7562 ("TRACE"), as part of the DARPA Transparent
 * Computing (TC) research program.
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
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
// TODO: separate this out
#include "llvm/IR/DebugInfoMetadata.h"

#include "llvm/Support/raw_ostream.h"
#include <sstream>
#include <string>

using namespace llvm;
using namespace loom;
using namespace std;

unique_ptr<Instrumenter> Instrumenter::Create(Module &Mod, NameFn NF,
                                              unique_ptr<InstrStrategy> S) {
  return unique_ptr<Instrumenter>(new Instrumenter(Mod, NF, std::move(S)));
}

Instrumenter::Instrumenter(llvm::Module &Mod, NameFn NF,
                           unique_ptr<InstrStrategy> S)
    : Mod(Mod), Strategy(std::move(S)), Name(NF) {}

bool Instrumenter::Instrument(llvm::Instruction *I, loom::Metadata Md, std::vector<loom::Transform> Transforms) {
  // If this instruction terminates a block, we need to treat it a bit
  // differently, placing instrumentation before it rather than after.
  const bool Terminator = I->isTerminator();

  // If this instruction is a terminator *or* is of void type, it doesn't
  // have a value for us to log.
  const bool Void = Terminator or I->getType()->isVoidTy();

  auto *OpcodeTy = IntegerType::get(Mod.getContext(), 32);
  unsigned Opcode = I->getOpcode();

  ParamVec ValueDescriptions = {{"opcode", OpcodeTy}};
  vector<Value *> Values = {ConstantInt::get(OpcodeTy, Opcode)};

  if (not Void) {
    ValueDescriptions.emplace_back(I->getName(), I->getType());
    Values.push_back(I);
  }

  for (Use &U : I->operands()) {
    // If this is a phi node, ignore the operands (values that *could* exist)
    // and stick to the phi value (that value that *does* exist).
    if (isa<PHINode>(I)) {
      break;
    }

    Value *V = U.get();

    // Don't try to pass a BasicBlock around by value (e.g., in a BranchInst).
    if (isa<BasicBlock>(V)) {
      continue;
    }

    // Don't use the address of an LLVM intrinsic: report its name instead.
    if (Function *F = dyn_cast<Function>(V)) {
      if (F->getName().startswith("llvm.")) {
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

  bool Varargs = false;
  if (Function *F = dyn_cast<Function>(I)) {
    Varargs = F->isVarArg();
  }

  // Instrument all non-terminators after the instructor, so that we can
  // capture the instruction's value (if non-void).
  const bool AfterInst = not Terminator;

  // Every instrumentation point needs a unique name. Eventually we should do
  // something clever with debug information, but for now we'll use a more...
  // simplistic approach.
  std::ostringstream NameBuilder;
  NameBuilder << "instrumentation:instruction:";
  NameBuilder << static_cast<const void *>(I);
  const string Name = NameBuilder.str();

  Strategy->Instrument(I, Name, Name, ValueDescriptions, Values, Md, Transforms,
						Varargs, AfterInst, true);

  return true;
}

// Currently, the second parameter is unused. It would give us the source name
// instead of the bitcode name
bool Instrumenter::InstrumentPtrInsts(llvm::Instruction *I,
                                      const llvm::DIVariable *Var,
									  loom::Metadata Md, 
									  std::vector<loom::Transform> Transforms) {

  std::ostringstream LocationBuilder;

  Value *Ptr = nullptr;
  Value *Val = nullptr;
  if (StoreInst *store = dyn_cast<StoreInst>(I)) {
    Ptr = store->getPointerOperand();
    Val = store->getValueOperand();
  } else if (LoadInst *load = dyn_cast<LoadInst>(I)) {
    Ptr = load->getPointerOperand();
    Val = load;
  } else if (GetElementPtrInst *gep = dyn_cast<GetElementPtrInst>(I)) {
    Ptr = gep->getPointerOperand();
    Val = gep;
  } else if (BitCastInst *bc = dyn_cast<BitCastInst>(I)) {
    Ptr = bc->getOperand(0);
    Val = bc;
  }

  StringRef PtrName = "";
  if (Ptr) {
    string dummy;
    raw_string_ostream raw(dummy);

    // Maintain the convention that return comes first
    Val->getType()->print(raw, false, true);
    raw << "| |";
    Ptr->getType()->print(raw, false, true);
    string out = raw.str();

    // Put in this order to not disturb snd by first inserting at 0
    size_t pos = 0;
    while ((pos = out.find('%', pos)) != std::string::npos) {
      out.insert(pos, "%");
      pos += 2;
    }
    LocationBuilder << '|' << out << "| ";

    PtrName = Ptr->getName();
  }
  if (!PtrName.empty()) {
    string Prefix = isa<GlobalValue>(*Ptr) ? "@" : "%%";
    LocationBuilder << Prefix << PtrName.str();
  } else {
    LocationBuilder << "???";
  }
  LocationBuilder << " ";
  LocationBuilder << I->getOpcodeName();
  LocationBuilder << ":";

  string FormatStringPrefix = LocationBuilder.str();

  // If this instruction terminates a block, we need to treat it a bit
  // differently, placing instrumentation before it rather than after.
  const bool Terminator = I->isTerminator();

  // If this instruction is a terminator *or* is of void type, it doesn't
  // have a value for us to log.
  const bool Void = Terminator or I->getType()->isVoidTy();

  ParamVec ValueDescriptions;
  vector<Value *> Values;

  if (not Void) {
    ValueDescriptions.emplace_back(I->getName(), I->getType());
    Values.push_back(I);
  } else {
    FormatStringPrefix.append(" %%void%%");
  }

  for (Use &U : I->operands()) {
    // If this is a phi node, ignore the operands (values that *could* exist)
    // and stick to the phi value (that value that *does* exist).
    if (isa<PHINode>(I)) {
      break;
    }

    Value *V = U.get();

    // Don't try to pass a BasicBlock around by value (e.g., in a BranchInst).
    if (isa<BasicBlock>(V)) {
      continue;
    }

    // Don't use the address of an LLVM intrinsic: report its name instead.
    if (Function *F = dyn_cast<Function>(V)) {
      if (F->getName().startswith("llvm.")) {
        V = IRBuilder<>(I).CreateGlobalStringPtr(F->getName(), "fn_name");
      }
    }

    Type *T = V->getType();
    if (T->isMetadataTy()) {
      // Ignore metadata-consuming instructions such as calls to
      // @llvm.dbg.declare().
      return false;
    }

    if (T->isPointerTy())
      // Remember, can condition on pointer but not && on it
      if (ConstantExpr *K = dyn_cast<ConstantExpr>(V)) {

        Instruction *N = K->getAsInstruction();
        if (isa<BitCastInst>(N) || isa<GetElementPtrInst>(N)) {
          N->insertBefore(I);
          this->InstrumentPtrInsts(N, nullptr);
          // Cannot remove because the instrumentation will use its result
        }
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

  std::ostringstream NameBuilder;
  NameBuilder << static_cast<const void *>(I);
  const string InstrName = Name({"instruction", NameBuilder.str()});

  Strategy->Instrument(I, InstrName, FormatStringPrefix, ValueDescriptions,
                       Values, Md, Transforms, Varargs, AfterInst, true);

  return true;
}

bool Instrumenter::Instrument(CallInst *Call, const Policy::Directions &D,
		loom::Metadata Md, std::vector<loom::Transform> Transforms) {
  bool ModifiedIR = false;

  for (auto Dir : D) {
    ModifiedIR |= Instrument(Call, Dir, Md, Transforms);
  }

  return ModifiedIR;
}

bool Instrumenter::Instrument(llvm::CallInst *Call, Policy::Direction Dir,
		loom::Metadata Md, std::vector<loom::Transform> Transforms) {
  Function *Target = Call->getCalledFunction();
  assert(Target); // TODO: support indirect targets, too

  // Get some relevant details about the call and the target function.
  const string TargetName = Target->getName().str();
  const bool VarArgs = Target->isVarArg();
  Type *CallType = Call->getType();
  const bool voidFunction = CallType->isVoidTy();
  const bool Return = (Dir == Policy::Direction::Out);

  const string Description = Return ? "return" : "call";
  const string FormatStringPrefix = Description + " " + TargetName + ":";

  std::vector<string> NameComponents = {Description, TargetName};
  // To avoid creating overloaded callout functions
  string Suffix;
  if (VarArgs) {
	  const void *address = static_cast<const void*>(Call);
	  std::stringstream ss;
	  ss << address;
	  Suffix = ss.str();
	  NameComponents.push_back(Suffix);
  }
  const string InstrName = Name(NameComponents);

  // Start by copying static and dynamic value details from the target function.
  //  If the function is variatric we used the types of the arguments.
  ParamVec Parameters;
  if (VarArgs) {
	for (auto &Arg : make_range(Call->arg_begin(), Call->arg_end())) {
		Parameters.emplace_back(Arg->getName(), Arg->getType());
	}
  } else {
	Parameters  = GetParameters(Target);
  }
  vector<Value *> Arguments(Call->getNumArgOperands());
  std::copy(Call->arg_begin(), Call->arg_end(), Arguments.begin());

  // The return value, if present, comes first in the instrumentation.
  if (Return and not voidFunction) {
    Parameters.emplace(Parameters.begin(), "retval", Call->getType());
    Arguments.emplace(Arguments.begin(), Call);
  }

  bool InstrAfterCall = Return;
  Strategy->Instrument(Call, InstrName, FormatStringPrefix, Parameters,
                       Arguments, Md, Transforms, VarArgs, InstrAfterCall);

  return true;
}

bool Instrumenter::Instrument(Function &Fn, const Policy::Directions &D,
                              loom::Metadata Md, std::vector<loom::Transform> Transforms) {
  bool ModifiedIR = false;

  for (auto Dir : D) {
    ModifiedIR |= Instrument(Fn, Dir, Md, Transforms);
  }

  return ModifiedIR;
}

bool Instrumenter::Instrument(Function &Fn, Policy::Direction Dir,
                              loom::Metadata Md, std::vector<loom::Transform> Transforms) {
  const bool Return = (Dir == Policy::Direction::Out);
  const string Description = Return ? "leave" : "enter";
  StringRef FnName = Fn.getName();
  const bool VarArgs = Fn.isVarArg();

  if (VarArgs) {
    errs() << "Warning: Trying to instrument vararg function " << FnName <<  ". Loom does not support instrumenting vararg functions.\n";
	return false;
  }

  assert(isa<PointerType>(Fn.getType()));
  FunctionType *FnType = dyn_cast<FunctionType>(Fn.getType()->getElementType());
  assert(FnType);
  const bool voidFunction = FnType->getReturnType()->isVoidTy();

  vector<Value *> Arguments;
  vector<Parameter> InstrParameters;

  if (Return and not voidFunction) {
    Arguments.push_back(nullptr);
    InstrParameters.emplace_back("retval", FnType->getReturnType());
  }

  for (auto &Arg : Fn.args()) {
    Arguments.push_back(&Arg);
    InstrParameters.emplace_back(Arg.getName(), Arg.getType());
  }

  const string InstrName = Name({Description, FnName.str()});
  string FormatStringPrefix = (Description + " " + FnName + ":").str();

  if (Return) {
    // Instrument all returns from the function:
    SmallVector<ReturnInst *, 4> Returns;

    for (auto &Block : Fn) {
      Instruction *Terminator = Block.getTerminator();
      if (auto *Ret = dyn_cast<ReturnInst>(Terminator)) {
        Returns.push_back(Ret);
      }
    }

    for (ReturnInst *Ret : Returns) {

      // This is a return: pass the return value and then the function's
      // parameters to the instrumentation.
      if (not voidFunction) {
        Arguments[0] = Ret->getReturnValue();
      } else {
        FormatStringPrefix.append(" %%void%%");
      }

      Strategy->Instrument(Ret, InstrName, FormatStringPrefix, InstrParameters,
                           Arguments, Md, Transforms, VarArgs);
    }

  } else {
    // Instrument the function's preamble:
    assert(not Fn.getBasicBlockList().empty());
    BasicBlock &Entry = Fn.getBasicBlockList().front();

    Strategy->Instrument(&Entry.front(), InstrName, FormatStringPrefix,
                         InstrParameters, Arguments, Md, Transforms, VarArgs);
  }

  return true;
}

bool Instrumenter::Instrument(GetElementPtrInst *GEP, LoadInst *Load,
                              StringRef FieldName, loom::Metadata Md, 
							  std::vector<loom::Transform> Transforms) {
  StructType *SourceType = dyn_cast<StructType>(GEP->getSourceElementType());
  assert(SourceType);
  assert(SourceType->getName().startswith("struct."));
  const string StructName = SourceType->getName().str().substr(7);

  ParamVec Parameters{
      {"source", GEP->getPointerOperandType()},
      {"value", Load->getType()},
  };

  vector<Value *> Arguments{
      GEP->getPointerOperand(),
      Load,
  };

  const string InstrName =
      Name({"load", "struct", StructName, "field", FieldName.str()});

  const string FormatStringPrefix =
      (StructName + "." + FieldName + " load:").str();

  Strategy->Instrument(Load, InstrName, FormatStringPrefix, Parameters,
                       Arguments, Md, Transforms, false, true);

  return true;
}

bool Instrumenter::Instrument(GetElementPtrInst *GEP, StoreInst *Store,
                              StringRef FieldName, loom::Metadata Md,
							  std::vector<loom::Transform> Transforms) {
  StructType *SourceType = dyn_cast<StructType>(GEP->getSourceElementType());
  assert(SourceType);
  assert(SourceType->getName().startswith("struct."));
  const string StructName = SourceType->getName().str().substr(7);

  ParamVec Parameters{
      {"source", GEP->getPointerOperandType()},
      {"value", Store->getValueOperand()->getType()},
  };

  vector<Value *> Arguments{
      GEP->getPointerOperand(),
      Store->getValueOperand(),
  };

  const string InstrName =
      Name({"store", "struct", StructName, "field", FieldName.str()});

  const string FormatStringPrefix =
      (StructName + "." + FieldName + " store:").str();

  Strategy->Instrument(Store, InstrName, FormatStringPrefix, Parameters,
                       Arguments, Md, Transforms);

  return true;
}

bool Instrumenter::InitializeLoggers(llvm::Function &Main) {

  assert(Main.getName().startswith("main"));
  return Strategy->Initialize(Main);
}

CallInst *Instrumenter::Extend(CallInst *Call, StringRef NewName,
                               ArrayRef<Value *> NewArgs,
                               ParamPosition Position) {
  Function *F = Call->getCalledFunction();
  FunctionType *FT = F->getFunctionType();

  // Create the new function and call by adding parameters and arguments
  // to the front or the back of the existing arguments and parameters.
  std::vector<Type *> ParamTypes = FT->params();
  SmallVector<Value *, 4> Arguments(Call->arg_begin(), Call->arg_end());

  for (Value *Arg : NewArgs) {
    if (Position == ParamPosition::End) {
      Arguments.push_back(Arg);
      ParamTypes.push_back(Arg->getType());
    } else {
      Arguments.insert(Arguments.begin(), Arg);
      ParamTypes.insert(ParamTypes.begin(), Arg->getType());
    }
  }

  FunctionCallee NewFn = Mod.getOrInsertFunction(
      NewName,
      FunctionType::get(FT->getReturnType(), ParamTypes, FT->isVarArg()));

  // Create the new call to the new function and replace the old call with it.
  CallInst *NewCall = CallInst::Create(NewFn, Arguments);
  ReplaceInstWithInst(Call, NewCall);

  return NewCall;
}


bool Instrumenter::ReplaceCall(llvm::CallInst *Call, llvm::StringRef NewCall) {

	ArrayRef<Value *> Empty;
	ParamPosition End = ParamPosition::End;

	Extend(Call, NewCall, Empty, End);

	return true;
}

bool Instrumenter::DeleteInst(llvm::Instruction *Inst) {

	Inst->eraseFromParent();

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
