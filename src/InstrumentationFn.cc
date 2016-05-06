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
  // Invariant: instrumentation functions should have a "preamble" block,
  // which contains instructions to execute first for all instrumented events
  // (e.g., logging calls) and an "exit" block after all actions required by
  // the instrumentation have been taken.
  //
  // The function starts out with the preamble branching to the exit block.
  // Instrumentation is added in new BasicBlocks in between.
  //
  BasicBlock *Preamble, *EndBlock;

  if (InstrFn->empty()) {
    Preamble = BasicBlock::Create(Ctx, "preamble", InstrFn);
    EndBlock = BasicBlock::Create(T->getContext(), "exit", InstrFn);

    IRBuilder<>(Preamble).CreateBr(EndBlock);
    IRBuilder<>(EndBlock).CreateRetVoid();

  } else {
    Preamble = FindBlock("preamble", *InstrFn);
    EndBlock = FindBlock("exit", *InstrFn);
  }

  return unique_ptr<InstrumentationFn> {
    new InstrumentationFn(InstrFn, Preamble, EndBlock)
  };
}


IRBuilder<> InstrumentationFn::GetPreambleBuilder()
{
  return IRBuilder<>(Preamble->getTerminator());
}


IRBuilder<> InstrumentationFn::AddAction(StringRef Name)
{
  // Get the last block in the instrumentation chain before "end"
  // and unhook it from "end": we need to insert the new block in between.
  BasicBlock *Predecessor = End->getSinglePredecessor();
  assert(Predecessor);

  End->removePredecessor(Predecessor);
  Predecessor->getTerminator()->eraseFromParent();

  // Create the new instrumentation block and insert it between the
  // old last-but-one block and the "end" block.
  LLVMContext& Ctx = InstrFn->getContext();
  BasicBlock *BB = BasicBlock::Create(Ctx, Name, InstrFn, End);

  IRBuilder<>(Predecessor).CreateBr(BB);
  BranchInst *Terminator = IRBuilder<>(BB).CreateBr(End);

  // Return an IRBuilder positioned immediately before the final branch
  // to the "end" block.
  return IRBuilder<>(Terminator);
}


void InstrumentationFn::CallBefore(Instruction *I, ArrayRef<Value*> Args) {
  CallInst::Create(InstrFn, Args)->insertBefore(I);
}

void InstrumentationFn::CallAfter(Instruction *I, ArrayRef<Value*> Args) {
  CallInst::Create(InstrFn, Args)->insertAfter(I);
}


Function::ArgumentListType& InstrumentationFn::GetParameters() {
  return InstrFn->getArgumentList();
}
