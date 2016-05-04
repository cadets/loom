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
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace loom;
using std::string;
using std::vector;


namespace {
  struct OptPass : public ModulePass {
    static char ID;
    OptPass() : ModulePass(ID) {}

    bool runOnModule(Module&) override;
  };
}


bool OptPass::runOnModule(Module &Mod)
{
  ErrorOr<std::unique_ptr<PolicyFile>> policyFile = PolicyFile::Open();
  if (std::error_code err = policyFile.getError()) {
    errs() << "Error opening LOOM policy file: " << err.message() << "\n";
    return false;
  }

  assert(*policyFile);
  Policy& policy = **policyFile;

  std::map<CallInst*, vector<Policy::Direction>> callInstsMap;
  InstrumentationFn::findAllCallInsts(&callInstsMap, Mod, policy);

  //Iterate over call instructions and create a new instrumented function
  //and create a function call to that new function
  for (auto i = callInstsMap.begin(); i != callInstsMap.end(); ++i) {
    CallInst* call = i->first;

    vector<Value*> Arguments;
    for (Use *m = call->arg_begin(), *n = call->arg_end(); m != n; ++m) {
        Arguments.push_back(m->get());
    }

    Function* Target = call->getCalledFunction();
    assert(Target); // TODO: support indirect targets, too
    const string TargetName = Target->getName();

    vector<Policy::Direction> directions = i->second;
    for(auto d = directions.begin(); d !=directions.end(); ++d) {
      vector<string> InstrNameComponents;

      switch (*d) {
      case Policy::Direction::In:
        InstrNameComponents.push_back("call");
        break;

      case Policy::Direction::Out:
        InstrNameComponents.push_back("return");
      }

      InstrNameComponents.push_back(TargetName);
      const string InstrName = policy.InstrName(InstrNameComponents);

      // Call instrumentation cannot be done entirely within a translation unit,
      // as there may be other calls to the same function in other units.
      GlobalValue::LinkageTypes Linkage = Function::ExternalLinkage;

      vector<Type*> ParameterTypes = InstrumentationFn::getParameterTypes(Target);

      std::unique_ptr<InstrumentationFn> InstrFn =
        InstrumentationFn::Create(InstrName, ParameterTypes, Linkage, Mod);

#if 0
  InstrumentationFn::setArgumentNames(target, pNewF);

  //Create a basic block and add printf call
  BasicBlock *block = BasicBlock::Create(module.getContext(), "entry", pNewF);
  IRBuilder<> Builder(block);
  InstrumentationFn::addPrintfCall(Builder, target, module, arguments);
  Builder.CreateRetVoid();
#endif

      InstrFn->CallBefore(call, Arguments);
      //callToInstr->setAttributes(target->getAttributes());
    }
  }
  return true;
}


char OptPass::ID = 0;
static RegisterPass<OptPass> X("loom", "Loom instrumentation", false, false);
