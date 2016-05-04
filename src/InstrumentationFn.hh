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
    Create(llvm::StringRef Name, llvm::ArrayRef<llvm::Type*> ParameterTypes,
           llvm::GlobalValue::LinkageTypes, llvm::Module&);

  /// Insert a call to this instrumentation just before the given Instruction.
  void CallBefore(llvm::Instruction*, llvm::ArrayRef<llvm::Value*> Args);

  /// Insert a call to this instrumentation just after the given Instruction.
  void CallAfter(llvm::Instruction*, llvm::ArrayRef<llvm::Value*> Args);

  /// Get the parameter types of the passed function.
  static std::vector<llvm::Type*> getParameterTypes(llvm::Function *pOldF);

  ///
  static void setArgumentNames(llvm::Function *pOldF, llvm::Function *pNewF);

  ///
  static void addPrintfCall(llvm::IRBuilder<> Builder, llvm::Function *pOldF,
                            llvm::Module &module,
                            std::vector<llvm::Value*> argumentValues);

  ///
  static void findAllCallInsts(std::map<llvm::CallInst*,
                               std::vector<Policy::Direction>> *callInstsMap,
                               llvm::Module &module, Policy &policy);

  // TODO: drop this if ObjC doesn't absolutely need it
  llvm::Function* getImplementation() { return InstrFn; }


private:
  InstrumentationFn(llvm::Function *InstrFn, llvm::BasicBlock *End)
    : InstrFn(InstrFn), End(End)
  {
  }

  llvm::Function *InstrFn;      //!< The instrumentation function.
  llvm::BasicBlock *End;        //!< End of the instrumentation chain.
};

}

#endif  /* !LOOM_INSTRUMENTATION_FN_H */
