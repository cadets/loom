//! @file InstrStrategy.cc  Definition of @ref loom::InstrStrategy.
/*
 * Copyright (c) 2016 Jonathan Anderson
 * All rights reserved.
 *
 * This software was developed by BAE Systems, the University of Cambridge
 * Computer Laboratory, and Memorial University under DARPA/AFRL contract
 * FA8650-15-C-7558 ("CADETS"), as part of the DARPA Transparent Computing
 * (TC) research program, as well as at Memorial University under the
 * NSERC Discovery program (RGPIN-2015-06048).
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
  CalloutStrategy(unique_ptr<Logger> Log)
    : InstrStrategy(std::move(Log))
  {
  }

  Instrumentation Instrument(Instruction *I, StringRef Name, StringRef Descrip,
                             ArrayRef<Parameter> Params,
                             ArrayRef<Value*> Values,
                             bool VarArgs, bool AfterInst) override;
};

} // anonymous namespace

#if defined(__unreachable)
  #define unreachable() __unreachable()
#elif __has_builtin(__builtin_unreachable)
  #define unreachable() __builtin_unreachable()
#else
  #define unreachable()
#endif


unique_ptr<InstrStrategy> InstrStrategy::Create(Kind K, unique_ptr<Logger> Log)
{
  switch (K) {
  case InstrStrategy::Kind::Callout:
    return unique_ptr<InstrStrategy>(new CalloutStrategy(std::move(Log)));
  }

  unreachable();
}


Instrumentation
CalloutStrategy::Instrument(Instruction *I, StringRef Name, StringRef Descrip,
                            ArrayRef<Parameter> Params, ArrayRef<Value*> Values,
                            bool VarArgs, bool AfterInst) {

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
  auto &InstrParams = InstrFn->getArgumentList();
  SmallVector<Value*, 4> InstrValues(InstrParams.size());

  std::transform(InstrParams.begin(), InstrParams.end(), InstrValues.begin(),
                 [](Argument &Arg) { return &Arg; });

  //
  // Invariant: instrumentation functions, if defined, should have a
  // "preamble" block, which contains instructions to execute first for
  // all instrumented events (e.g., logging calls) and an "exit" block
  // after all actions required by the instrumentation have been taken.
  //
  // The function starts out with the preamble branching to the exit block.
  // Instrumentation is added in new BasicBlocks in between.
  //
  BasicBlock *Preamble, *EndBlock;

  if (InstrFn->empty()) {
    InstrFn->setLinkage(Function::InternalLinkage);

    Preamble = BasicBlock::Create(Ctx, "preamble", InstrFn);
    EndBlock = BasicBlock::Create(T->getContext(), "exit", InstrFn);

    IRBuilder<> PreambleBuilder(Preamble);

    if (Log) {
      Log->Call(PreambleBuilder, Descrip, InstrValues, "\n");
    }

    PreambleBuilder.CreateBr(EndBlock);
    IRBuilder<>(EndBlock).CreateRetVoid();

    // Also set instrumentation function's parameter names:
    SymbolTableList<Argument>& ArgList = InstrFn->getArgumentList();
    size_t i = 0;
    for (auto a = ArgList.begin(); a != ArgList.end(); a++) {
      a->setName(Params[i].first);
      i++;
    }

  } else {
    Preamble = FindBlock("preamble", *InstrFn);
    EndBlock = FindBlock("exit", *InstrFn);
  }

  // Call the instrumentation function:
  CallInst *Call = CallInst::Create(InstrFn, Values);
  Call->dump();

  if (AfterInst) {
    Call->insertAfter(I);
  } else {
    Call->insertBefore(I);
  }

  if (auto *CI = dyn_cast<CallInst>(I)) {
    if (Function *Target = CI->getCalledFunction()) {
      Call->setAttributes(Target->getAttributes());
    }
  }

  return Instrumentation(InstrValues, Preamble, EndBlock);
}
