//! @file InstrStrategy.cc  Definition of @ref loom::InstrStrategy.
/*
 * Copyright (c) 2016-2017 Jonathan Anderson
 * All rights reserved.
 *
 * This software was developed by BAE Systems, the University of Cambridge
 * Computer Laboratory, and Memorial University under DARPA/AFRL contract
 * FA8650-15-C-7558 ("CADETS"), as part of the DARPA Transparent Computing
 * (TC) research program, as well as at Memorial University under the
 * NSERC Discovery program (RGPIN-2015-06048).
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

#include <llvm/IR/Module.h>

#include "Instrumentation.hh"
#include "InstrStrategy.hh"

using namespace llvm;
using namespace loom;
using std::unique_ptr;


namespace {

class CalloutStrategy : public InstrStrategy {
public:
  CalloutStrategy(bool UseBlocks)
    : InstrStrategy(UseBlocks)
  {
  }

  Instrumentation Instrument(Instruction *I, StringRef Name, StringRef Descrip,
                             ArrayRef<Parameter> Params,
                             ArrayRef<Value*> Values, StringRef Metadata,
                             bool VarArgs, bool AfterInst, bool) override;
};

class InlineStrategy : public InstrStrategy {
public:
  InlineStrategy(bool UseBlocks)
    : InstrStrategy(UseBlocks)
  {
  }

  Instrumentation Instrument(Instruction *I, StringRef Name, StringRef Descrip,
                             ArrayRef<Parameter> Params,
                             ArrayRef<Value*> Values, StringRef Metadata,
                             bool VarArgs, bool AfterInst,
                             bool SuppressInstrumentation) override;
};

} // anonymous namespace


unique_ptr<InstrStrategy> InstrStrategy::Create(Kind K, bool UseBlocks)
{
  switch (K) {
  case InstrStrategy::Kind::Callout:
    return unique_ptr<InstrStrategy>(new CalloutStrategy(UseBlocks));

  case InstrStrategy::Kind::Inline:
    return unique_ptr<InstrStrategy>(new InlineStrategy(UseBlocks));
  }
}


InstrStrategy::~InstrStrategy()
{
}


void InstrStrategy::AddLogger(unique_ptr<Logger> L) {
  assert(L);
  Loggers.emplace_back(std::move(L));
}


Value*
InstrStrategy::AddLogging(Instruction *I, ArrayRef<Value*> Values,
                          StringRef Name, StringRef Description,
                          StringRef Metadata, bool SuppressUniqueness) {
  Value *End = nullptr;

  for (auto& L : Loggers) {
    assert(L);
    End = L->Log(I, Values, Name, Description, Metadata, SuppressUniqueness);
  }

  return End;
}


Instrumentation
CalloutStrategy::Instrument(Instruction *I, StringRef Name, StringRef Descrip,
                            ArrayRef<Parameter> Params, ArrayRef<Value*> Values,
							StringRef Metadata, bool VarArgs, bool AfterInst, 
							bool SuppressUniq) {

  assert(Params.size() == Values.size());

  Module *M = I->getModule();
  LLVMContext& Ctx = M->getContext();

  std::vector<Type*> ParamTypes(Params.size());
  std::transform(Params.begin(), Params.end(), ParamTypes.begin(),
                 [](const Parameter& P) { return P.second; });

  auto *T = FunctionType::get(Type::getVoidTy(Ctx), ParamTypes, VarArgs);
  auto *InstrFn = dyn_cast<Function>(M->getOrInsertFunction(Name, T));

  //
  // The instrumented values, as accessed within the instrumentation, are the
  // instrumentation function's parameters rather than values that we we see
  // within this context. Convert these from llvm::Argument& to llvm::Value*.
  //
  SmallVector<Value*, 4> InstrValues;
  for (Argument &Arg : InstrFn->args()) {
    InstrValues.push_back(&Arg);
  }

  //
  // Find (if the instrumentation function already exists) or create (if it
  // does not) the "preamble" and "exit" blocks required by the invariant on
  // Instrumentation.
  //
  BasicBlock *Preamble, *EndBlock;
  Instruction *PreambleEnd, *End;

  if (InstrFn->empty()) {
    InstrFn->setLinkage(Function::InternalLinkage);

    if (UseBlockStructure) {
      Preamble = BasicBlock::Create(Ctx, "preamble", InstrFn);
      EndBlock = BasicBlock::Create(T->getContext(), "exit", InstrFn);

      PreambleEnd = IRBuilder<>(Preamble).CreateBr(EndBlock);
      End = IRBuilder<>(EndBlock).CreateRetVoid();

    } else {
      Preamble = BasicBlock::Create(Ctx, "entry", InstrFn);;
      EndBlock = nullptr;

      PreambleEnd = IRBuilder<>(Preamble).CreateRetVoid();
      End = PreambleEnd;
    }

    AddLogging(PreambleEnd, InstrValues, Name, Descrip, Metadata, SuppressUniq);

    // Also set instrumentation function's parameter names:
    size_t i = 0;
    for (Argument &Arg : InstrFn->args()) {
      Arg.setName(Params[i].first);
      i++;
    }

  } else {
    Preamble = FindBlock("preamble", *InstrFn);
    EndBlock = FindBlock("exit", *InstrFn);
  }

  // Call the instrumentation function:
  CallInst *Call = CallInst::Create(InstrFn, Values);

  if (AfterInst) {
    Call->insertAfter(I);
  } else {
    Call->insertBefore(I);
  }

  if (UseBlockStructure) {
    return Instrumentation(InstrValues, Preamble, EndBlock, PreambleEnd, End);
  } else {
    return Instrumentation(InstrValues, PreambleEnd, End);
  }
}


Instrumentation
InlineStrategy::Instrument(Instruction *I, StringRef Name, StringRef Descrip,
                           ArrayRef<Parameter> Params, ArrayRef<Value*> Values,
						   StringRef Metadata, bool VarArgs, bool AfterInst, 
						   bool SuppressUniq) {

  BasicBlock *BB = I->getParent();
  Function *F = BB->getParent();
  Module *M = F->getParent();
  LLVMContext& Ctx = M->getContext();

  if (AfterInst) {
    // Move to the next instruction.
    BasicBlock::iterator Iter(I);
    Iter++;

    // It is illegal to create a degenerate BasicBlock, so we can't insert
    // instrumentation after a BasicBlock's terminator. Callers should never
    // attempt to, e.g., instrument a ReturnInst with AfterInst = true.
    assert(Iter != BB->end() && "instrumenting after BB's final instruction");

    I = &*Iter;
  }

  BasicBlock *Preamble, *EndBlock;
  Instruction *PreambleEnd, *End;

  if (UseBlockStructure) {
    // Split the target instruction's BasicBlock and insert our instrumentation
    // blocks (preamble and end) in the split.
    BasicBlock *PostSplit = BB->splitBasicBlock(I);
    BB->getTerminator()->eraseFromParent();

    EndBlock = BasicBlock::Create(Ctx, Name + ":end", F, PostSplit);
    End = IRBuilder<>(EndBlock).CreateBr(PostSplit);

    Preamble = BasicBlock::Create(Ctx, Name + ":preamble", F, EndBlock);
    PreambleEnd = IRBuilder<>(Preamble).CreateBr(EndBlock);

    IRBuilder<>(BB).CreateBr(Preamble);

  } else {
    Preamble = nullptr;
    EndBlock = nullptr;

    PreambleEnd = I;
    End = I;
  }

  AddLogging(PreambleEnd, Values, Name, Descrip, Metadata, SuppressUniq);

  SmallVector<Value*, 4> V(Values.begin(), Values.end());

  if (Preamble and EndBlock) {
    return Instrumentation(V, Preamble, EndBlock, PreambleEnd, End);
  } else {
    return Instrumentation(V, PreambleEnd, End);
  }
}
