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

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

using namespace llvm;
using namespace loom;

using std::string;
using std::unique_ptr;
using std::vector;


static BasicBlock *FindBlock(StringRef Name, Function& Fn) {
  for (auto& B : Fn)
    if (B.getName() == Name)
      return &B;

  return NULL;
}


unique_ptr<InstrumentationFn>
InstrumentationFn::Create(StringRef Name, ArrayRef<Parameter> Parameters,
                          GlobalValue::LinkageTypes Linkage, Module& M) {

  LLVMContext& Ctx = M.getContext();

  vector<Type*> ParamTypes;
  for (auto& P : Parameters) {
    ParamTypes.push_back(P.second);
  }

  FunctionType *T = FunctionType::get(Type::getVoidTy(Ctx), ParamTypes, false);
  auto *InstrFn = dyn_cast<Function>(M.getOrInsertFunction(Name, T));
  InstrFn->setLinkage(Linkage);

  // Set instrumentation function's parameter names:
  SymbolTableList<Argument>& ArgList = InstrFn->getArgumentList();
  size_t i = 0;
  for (auto a = ArgList.begin(); a != ArgList.end(); a++) {
    a->setName(Parameters[i].first);
    i++;
  }

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
