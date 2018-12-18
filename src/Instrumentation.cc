//! @file Instrumentation.cc  Definition of @ref loom::Instrumentation.
/*
 * Copyright (c) 2013,2015-2016 Jonathan Anderson
 * Copyright (c) 2016 Cem Kilic
 * All rights reserved.
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory under DARPA/AFRL contract (FA8750-10-C-0237)
 * ("CTSRD"), as part of the DARPA CRASH research programme, by BAE Systems,
 * the University of Cambridge Computer Laboratory, and Memorial University
 * under DARPA/AFRL contract FA8650-15-C-7558 ("CADETS"), as part of the
 * DARPA Transparent Computing (TC) research program, and at Memorial University
 * under the NSERC Discovery program (RGPIN-2015-06048).
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

#include "Instrumentation.hh"
#include "llvm/IR/TypeBuilder.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

using namespace llvm;
using namespace loom;

using std::string;
using std::unique_ptr;
using std::vector;

Instrumentation::Instrumentation(SmallVector<Value *, 4> Values,
                                 BasicBlock *Preamble, BasicBlock *EndBlock,
                                 Instruction *Begin, Instruction *End)
    : InstrValues(std::move(Values)), Preamble(Preamble), EndBlock(EndBlock),
      PreambleEnd(Begin), End(End) {
  assert(not Preamble->empty());
  assert(not EndBlock->empty());
}

Instrumentation::Instrumentation(SmallVector<Value *, 4> Values,
                                 Instruction *Begin, Instruction *End)
    : InstrValues(std::move(Values)), Preamble(nullptr), EndBlock(nullptr),
      PreambleEnd(Begin), End(End) {}

IRBuilder<> Instrumentation::GetBuilder() {
  assert(End);
  return IRBuilder<>(End);
}

IRBuilder<> Instrumentation::AddAction(StringRef Name) {
  assert(PreambleEnd && End);

  if (not Preamble) {
    // If we don't have a preamble block, we are doing direct inline
    // instrumentation and we should not have an end block either.
    // Return an IRBuilder positioned just before the end instruction.
    assert(not EndBlock);
    return IRBuilder<>(End);
  }

  assert(EndBlock);

  // Get the last block in the instrumentation chain before "end"
  // and unhook it from "end": we need to insert the new block in between.
  BasicBlock *Predecessor = EndBlock->getSinglePredecessor();
  assert(Predecessor);

  EndBlock->removePredecessor(Predecessor);
  Predecessor->getTerminator()->eraseFromParent();

  // Create the new instrumentation block and insert it between the
  // old last-but-one block and the "end" block.
  Function *Fn = Predecessor->getParent();
  LLVMContext &Ctx = Fn->getContext();
  BasicBlock *BB = BasicBlock::Create(Ctx, Name, Fn, EndBlock);

  IRBuilder<>(Predecessor).CreateBr(BB);
  BranchInst *Terminator = IRBuilder<>(BB).CreateBr(EndBlock);

  // Return an IRBuilder positioned immediately before the final branch
  // to the "end" block.
  return IRBuilder<>(Terminator);
}
