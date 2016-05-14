//! @file InstrumentationFn.h  Declaration of @ref loom::InstrumentationFn.
/*
 * Copyright (c) 2012-2013,2015-2016 Jonathan Anderson
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

#ifndef  LOOM_INSTRUMENTATION_FN_H
#define  LOOM_INSTRUMENTATION_FN_H

#include "IRUtils.hh"
#include "PolicyFile.hh"

#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>

namespace llvm {
  class Instruction;
  class LLVMContext;
  class Module;
}

namespace loom {

/**
 * An instrumentation function that receives program events such as
 * "called foo(42,97)" and does something with them.
 */
class InstrumentationFn {
public:
  static std::unique_ptr<InstrumentationFn>
    Create(llvm::StringRef Name, llvm::ArrayRef<Parameter> ParameterDetails,
           llvm::GlobalValue::LinkageTypes, llvm::Module&,
           bool Define = false);

  /// Has this instrumentation been defined, or is it just a declaration?
  bool isDefined() const;

  /// Insert a call to this instrumentation just before the given Instruction.
  llvm::CallInst* CallBefore(llvm::Instruction*,
                             llvm::ArrayRef<llvm::Value*> Args);

  /// Insert a call to this instrumentation just after the given Instruction.
  llvm::CallInst* CallAfter(llvm::Instruction*,
                            llvm::ArrayRef<llvm::Value*> Args);

  /**
   * Retrieve the instrumentation function's parameters.
   *
   * The mapping from instrumented values to instrumentation function parameters
   * depends on the specifics of the instrumentation (e.g., call and return
   * instrumentation may differ in that return may see a return value).
   */
  llvm::Function::ArgumentListType& GetParameters();

  /**
   * Get an IRBuilder that can be used to insert new instructions into the
   * instrumentation's preamble block.
   *
   * The preamble is a block where we can put instructions that should run
   * whenever the instrumentation is called, regardless of the values that
   * are passed into it. This could be used to implement gprof-style counting
   * or simple logging.
   */
  llvm::IRBuilder<> GetPreambleBuilder();

  /// Add a new link in the chain of actions for this instrumentation.
  llvm::IRBuilder<> AddAction(llvm::StringRef Name);

  /// Retrieve the LLVM function that implements this instrumentation.
  llvm::Function* getImplementation() { return InstrFn; }

  /// The structure dereference operator returns the implentation function.
  llvm::Function* operator -> () { return getImplementation(); }


private:
  InstrumentationFn(llvm::Function *InstrFn, llvm::BasicBlock *Preamble,
                    llvm::BasicBlock *End)
    : InstrFn(InstrFn), Preamble(Preamble), End(End)
  {
  }

  llvm::Function *InstrFn;      //!< The instrumentation function.
  llvm::BasicBlock *Preamble;   //!< Cross-instrumentation logging, etc.
  llvm::BasicBlock *End;        //!< End of the instrumentation chain.
};

}

#endif  /* !LOOM_INSTRUMENTATION_FN_H */
