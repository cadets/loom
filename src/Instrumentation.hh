//! @file Instrumentation.hh  Declaration of @ref loom::Instrumentation.
/*
 * Copyright (c) 2012-2013,2015-2016 Jonathan Anderson
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

#ifndef LOOM_INSTRUMENTATION_FN_H
#define LOOM_INSTRUMENTATION_FN_H

#include "IRUtils.hh"
#include "InstrStrategy.hh"
#include "PolicyFile.hh"

#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>

namespace llvm {
class Instruction;
class LLVMContext;
class Module;
} // namespace llvm

namespace loom {

/**
 * Instrumentation code (which could be a function or inline instructions)
 * that receives program events such as "called foo(42,97)" and takes
 * policy-defined actions (e.g., logs events or updates counters).
 */
class Instrumentation {
public:
  /**
   * Construct some instrumentation that puts all actions into BasicBlocks
   * injected between the specified preamble and end blocks.
   */
  Instrumentation(llvm::SmallVector<llvm::Value *, 4> Values,
                  llvm::BasicBlock *Preamble, llvm::BasicBlock *EndBlock,
                  llvm::Instruction *PreambleEnd, llvm::Instruction *End);

  /**
   * Construct some instrumentation that directly injects actions as inline
   * instructions rather than using BasicBlocks.
   */
  Instrumentation(llvm::SmallVector<llvm::Value *, 4> Values,
                  llvm::Instruction *PreambleEnd, llvm::Instruction *End);

  //! Retrieve the values passed into the instrumentation.
  llvm::ArrayRef<llvm::Value *> Values() { return InstrValues; }

private:
  llvm::SmallVector<llvm::Value *, 4> InstrValues; //!< Instrumented values
  llvm::BasicBlock *Preamble;     //!< Cross-instrumentation logging, etc.
  llvm::BasicBlock *EndBlock;     //!< End of the instrumentation block chain.
  llvm::Instruction *PreambleEnd; //!< Terminator of the preamble.
  llvm::Instruction *End;         //!< Final instruction in the instrumentation.
};

} // namespace loom

#endif /* !LOOM_INSTRUMENTATION_FN_H */
