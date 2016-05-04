//! @file OptPass.cc  Definition of @ref loom::OptPass.
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


namespace {
  struct OptPass : public ModulePass {
    static char ID;
    OptPass() : ModulePass(ID) {}

    bool runOnModule(Module&) override;
  };
}


bool OptPass::runOnModule(Module &module)
{
  ErrorOr<std::unique_ptr<PolicyFile>> policyFile = PolicyFile::Open();
  if (std::error_code err = policyFile.getError()) {
    errs() << "Error opening LOOM policy file: " << err.message() << "\n";
    return false;
  }

  assert(*policyFile);
  Policy& policy = **policyFile;

  std::map<CallInst*, std::vector<Policy::Direction>> callInstsMap;
  InstrumentationFn::findAllCallInsts(&callInstsMap, module, policy);

  //Iterate over call instructions and create a new instrumented function
  //and create a function call to that new function
  for (auto i = callInstsMap.begin(); i != callInstsMap.end(); ++i) {
    CallInst* callInst = (*i).first;
    Function* pOldF = callInst->getCalledFunction();
    callInst->setCallingConv(pOldF->getCallingConv());
    std::vector<Value*> argumentValues;
    for (Use *m = callInst->arg_begin(), *n = callInst->arg_end(); m != n; ++m) {
        argumentValues.push_back(m->get());
    }

    std::vector<Policy::Direction> directions = i->second;
    for(auto it = directions.begin(); it !=directions.end(); ++it) {
      Function* pNewF = InstrumentationFn::createInstrFunction(module, (*i).first, *it, policy);
      IRBuilder<> Builder(&*callInst);
      CallInst *callToInstr = Builder.CreateCall(pNewF, argumentValues);
      callToInstr->setAttributes(pOldF->getAttributes());
    }
  }
  return true;
}


char OptPass::ID = 0;
static RegisterPass<OptPass> X("loom", "Loom instrumentation", false, false);
