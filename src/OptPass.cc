//! @file OptPass.cc  Definition of @ref loom::OptPass.
/*
 * Copyright (c) 2016 Jonathan Anderson
 * Copyright (c) 2016 Cem Kilic
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

#include "PolicyFile.hh"
#include "InstrumentationFn.hh"

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

#include <unordered_set>

using namespace llvm;
using namespace loom;
using std::string;
using std::vector;


namespace {
  struct OptPass : public ModulePass {
    static char ID;
    OptPass() : ModulePass(ID), PolFile(PolicyFile::Open()) {}

    bool runOnModule(Module&) override;

    /// Instrument a single call instruction, before and/or after according
    /// to the given vector of directions.
    bool Instrument(CallInst*, Policy&);

    llvm::ErrorOr<std::unique_ptr<PolicyFile>> PolFile;
  };
}

static std::vector<Type*> ParameterTypes(Function*);


bool OptPass::runOnModule(Module &Mod)
{
  if (std::error_code err = PolFile.getError()) {
    errs() << "Error opening LOOM policy file: " << err.message() << "\n";
    return false;
  }

  assert(*PolicyFile);
  Policy& P = **PolFile;

  bool ModifiedIR = false;

  //
  // First find all of the calls that need to be instrumented.
  // This will prevent us from invalidating iterators or
  // instrumenting our instrumentation.
  //
  std::unordered_set<CallInst*> Calls;

  for (auto& Fn : Mod) {
    for (auto& Inst : instructions(Fn)) {
      if (CallInst* Call = dyn_cast<CallInst>(&Inst)) {
        Function *Target = Call->getCalledFunction();
        if (Target and not P.CallInstrumentation(*Target).empty())
          Calls.insert(Call);
      }
    }
  }

  //
  // Instrument function calls:
  //
  for (auto *Call : Calls)
    ModifiedIR |= Instrument(Call, P);

  return ModifiedIR;
}


bool OptPass::Instrument(CallInst *Call, Policy& Pol) {

  Module& Mod = *Call->getModule();
  Type *CallType = Call->getType();
  const bool voidFunction = CallType->isVoidTy();

  vector<Value*> Arguments;
  for (Use *m = Call->arg_begin(), *n = Call->arg_end(); m != n; ++m) {
      Arguments.push_back(m->get());
  }

  Function* Target = Call->getCalledFunction();
  assert(Target); // TODO: support indirect targets, too
  assert(not Pol.CallInstrumentation(*Target).empty());
  const string TargetName = Target->getName();

  for(auto Dir : Pol.CallInstrumentation(*Target)) {
    vector<string> InstrNameComponents;
    vector<Type*> ParamTypes = ParameterTypes(Target);

    switch (Dir) {
    case Policy::Direction::In:
      InstrNameComponents.push_back("call");
      break;

    case Policy::Direction::Out:
      InstrNameComponents.push_back("return");
      if (not voidFunction)
        ParamTypes.insert(ParamTypes.begin(), Call->getType());
    }

    InstrNameComponents.push_back(TargetName);
    const string InstrName = Pol.InstrName(InstrNameComponents);

    // Call instrumentation can be done entirely within a translation unit:
    // calls in other units can use their own instrumentation functions.
    GlobalValue::LinkageTypes Linkage = Function::InternalLinkage;

    std::unique_ptr<InstrumentationFn> InstrFn =
      InstrumentationFn::Create(InstrName, ParamTypes, Linkage, Mod);

#if 0
InstrumentationFn::setArgumentNames(target, pNewF);

//Create a basic block and add printf call
BasicBlock *block = BasicBlock::Create(module.getContext(), "entry", pNewF);
IRBuilder<> Builder(block);
InstrumentationFn::addPrintfCall(Builder, target, module, arguments);
Builder.CreateRetVoid();
#endif

    switch (Dir) {
    case Policy::Direction::In:
      InstrFn->CallBefore(Call, Arguments);
      break;

    case Policy::Direction::Out:
      if (not voidFunction)
        Arguments.insert(Arguments.begin(), Call);
      InstrFn->CallAfter(Call, Arguments);
      break;
    }
    //callToInstr->setAttributes(target->getAttributes());
  }

  return true;
}

static std::vector<Type*> ParameterTypes(Function *Fn) {
  std::vector<Type*> Types;
  for(auto& Arg : Fn->getArgumentList()) {
    Types.push_back(Arg.getType());
  }
  return Types;
}

char OptPass::ID = 0;
static RegisterPass<OptPass> X("loom", "Loom instrumentation", false, false);
