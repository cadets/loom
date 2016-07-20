//! @file Instrumentation.cpp  Definition of @ref loom::Instrumentation.
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


IRBuilder<> Instrumentation::GetPreambleBuilder()
{
  assert(Preamble);
  return IRBuilder<>(Preamble->getTerminator());
}


IRBuilder<> Instrumentation::AddAction(StringRef Name)
{
  assert(Preamble && End);

  // Get the last block in the instrumentation chain before "end"
  // and unhook it from "end": we need to insert the new block in between.
  BasicBlock *Predecessor = End->getSinglePredecessor();
  assert(Predecessor);

  End->removePredecessor(Predecessor);
  Predecessor->getTerminator()->eraseFromParent();

  // Create the new instrumentation block and insert it between the
  // old last-but-one block and the "end" block.
  Function *Fn = Predecessor->getParent();
  LLVMContext& Ctx = Fn->getContext();
  BasicBlock *BB = BasicBlock::Create(Ctx, Name, Fn, End);

  IRBuilder<>(Predecessor).CreateBr(BB);
  BranchInst *Terminator = IRBuilder<>(BB).CreateBr(End);

  // Return an IRBuilder positioned immediately before the final branch
  // to the "end" block.
  return IRBuilder<>(Terminator);
}
