//! @file InstrumentationFn.cpp  Definition of @ref loom::InstrumentationFn.
/*
 * Copyright (c) 2013,2015-2016 Jonathan Anderson
 * Copyright (c) 2016 Cem Kilic
 * All rights reserved.
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory under DARPA/AFRL contract (FA8750-10-C-0237)
 * ("CTSRD"), as part of the DARPA CRASH research programme, as well as at
 * Memorial University under the NSERC Discovery program (RGPIN-2015-06048).
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

#include "InstrumentationFn.hh"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/IR/InstIterator.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

using namespace llvm;
using namespace loom;

using std::string;
using std::unique_ptr;


static BasicBlock *FindBlock(StringRef Name, Function& Fn) {
  for (auto& B : Fn)
    if (B.getName() == Name)
      return &B;

  return NULL;
}


unique_ptr<InstrumentationFn>
InstrumentationFn::Create(StringRef Name, ArrayRef<Type*> ParamTypes,
                          GlobalValue::LinkageTypes Linkage, Module& M) {

  LLVMContext& Ctx = M.getContext();

  FunctionType *T = FunctionType::get(Type::getVoidTy(Ctx), ParamTypes, false);
  auto *InstrFn = dyn_cast<Function>(M.getOrInsertFunction(Name, T));
  InstrFn->setLinkage(Linkage);

  //
  // Invariant: instrumentation functions should have two exit blocks, one for
  // normal termination and one for abnormal termination.
  //
  // The function starts out with the entry block jumping to the exit block.
  // Instrumentation is added in new BasicBlocks in between.
  //
  BasicBlock *EndBlock;

  if (InstrFn->empty()) {
    BasicBlock *Entry = BasicBlock::Create(Ctx, "entry", InstrFn);
    BasicBlock *Preamble = BasicBlock::Create(Ctx, "preamble", InstrFn);
    EndBlock = BasicBlock::Create(T->getContext(), "exit", InstrFn);

    IRBuilder<>(Entry).CreateBr(Preamble);
    IRBuilder<>(Preamble).CreateBr(EndBlock);
    IRBuilder<>(EndBlock).CreateRetVoid();

  } else
    EndBlock = FindBlock("exit", *InstrFn);

  return unique_ptr<InstrumentationFn> {
    new InstrumentationFn(InstrFn, EndBlock)
  };
}


void InstrumentationFn::CallBefore(Instruction *I, ArrayRef<Value*> Args) {
  CallInst::Create(InstrFn, Args)->insertBefore(I);
}

void InstrumentationFn::CallAfter(Instruction *I, ArrayRef<Value*> Args) {
  CallInst::Create(InstrFn, Args)->insertAfter(I);
}

std::vector<Type*> InstrumentationFn::getParameterTypes(Function *pOldF) {
  std::vector<Type*> paramTypes;
  SymbolTableList<Argument> *oldArgList = &(pOldF->getArgumentList());
  for(auto k = oldArgList->begin(), l = oldArgList->end(); k != l; k++) {
    paramTypes.push_back(k->getType());
  }
  return paramTypes;
}

void InstrumentationFn::setArgumentNames(Function *pOldF, Function *pNewF) {
  SymbolTableList<Argument> *oldArgList = &(pOldF->getArgumentList());
  for(ilist_iterator<Argument> k = oldArgList->begin(), l = oldArgList->end(), m = pNewF->getArgumentList().begin(); k != l; ++k, ++m) {
    m->setName(k->getName());
  }
}

void InstrumentationFn::addPrintfCall(IRBuilder<> Builder, Function *pOldF, Module &module, std::vector<Value*> argumentValues)
{
  FunctionType *printfFT =
    TypeBuilder<int(const char *, ...), false>::get(pOldF->getContext());

  Function *printf = Function::Create(printfFT, Function::ExternalLinkage,
                                      "printf", &module);

  std::vector<Value*> printfArgs;
  std::string calledFunctionName = pOldF->getName();
  std::string printfStringArg = "calling " + calledFunctionName + "(";
  for(auto it=argumentValues.begin(); it !=argumentValues.end(); ++it)
  {
    Type *argType = (*it)->getType();
    if(argType->isIntegerTy(32)) {
      printfStringArg.append("%d");

    } else if (argType->isFloatTy() || argType->isDoubleTy()) {
      printfStringArg.append("%.0f");

    } else if (argType->isIntegerTy(8)) {
      printfStringArg.append("%c");

    } else if (argType->isPointerTy()) {
      printfStringArg.append("%s");

    }

    if(*it==argumentValues.back()) {
      printfStringArg.append(")\n\"");

    } else {
      printfStringArg.append(", ");
    }
  }
  Value *arg1 = Builder.CreateGlobalStringPtr(printfStringArg);
  printfArgs.push_back(arg1);
  printfArgs.insert(printfArgs.end(), argumentValues.begin(), argumentValues.end());
  Builder.CreateCall(printf, printfArgs);
}

void InstrumentationFn::findAllCallInsts(std::map<CallInst*, std::vector<Policy::Direction>> *callInstsMap, Module &module, Policy& policy)
{
  for (Module::iterator i = module.begin(), e = module.end(); i!=e; i++) {
    for (inst_iterator j = inst_begin(*i), e = inst_end(*i); j != e; ++j) {
      if (CallInst* callInst = dyn_cast<CallInst>(&*j)) {
        auto directions = policy.CallInstrumentation(*callInst->getCalledFunction());
        if (not directions.empty())
        {
          callInstsMap->insert(std::make_pair(callInst, directions));
        }
      }
    }
  }
}
