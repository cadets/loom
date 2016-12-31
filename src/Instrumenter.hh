//! @file Instrumenter.hh  Declaration of @ref loom::Instrumenter.
/*
 * Copyright (c) 2016 Jonathan Anderson
 * All rights reserved.
 *
 * This software was developed by BAE Systems, the University of Cambridge
 * Computer Laboratory, and Memorial University under DARPA/AFRL contract
 * FA8650-15-C-7558 ("CADETS"), as part of the DARPA Transparent Computing
 * (TC) research program.
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

#ifndef LOOM_INSTRUMENTER_H_
#define LOOM_INSTRUMENTER_H_

#include "Instrumentation.hh"
#include "Policy.hh"

#include <functional>


namespace loom {

class InstrStrategy;

/// An object used for instrumenting code within a single LLVM module.
class Instrumenter {
public:
  /**
   * A @ref NameFn takes an ordered collection of name components (e.g.,
   * `"call"`, `"fn"`, `"foo"`) and converts it into a name that can
   * be used for instrumentation (e.g., `"__tesla_call_fn_foo"`).
   */
  typedef std::function<std::string (const std::vector<std::string>&)> NameFn;

  /**
   * Create a new Instrumenter instance.
   *
   * @param   NF      a function that can be called to name instrumentation
   *                  (e.g., ["call","foo"] => "__loom_hook_call_foo")
   */
  static std::unique_ptr<Instrumenter>
    Create(llvm::Module&, NameFn NF, std::unique_ptr<InstrStrategy>);

  /// Instrument an instruction generically: instruction name and values.
  bool Instrument(llvm::Instruction*);

  /// Instrument a function call in the call and/or return direction.
  bool Instrument(llvm::CallInst*, const Policy::Directions&);

  /// Instrument a function call (caller-side), either calling or returning.
  bool Instrument(llvm::CallInst *Call, Policy::Direction);

  /// Instrument a function entry and/or exit.
  bool Instrument(llvm::Function&, const Policy::Directions&);

  /// Instrument a function entry or exit.
  bool Instrument(llvm::Function&, Policy::Direction);

  /// Instrument a read from a structure field.
  bool Instrument(llvm::GetElementPtrInst*, llvm::LoadInst*,
                  llvm::StringRef FieldName);

  /// Instrument a write to a structure field.
  bool Instrument(llvm::GetElementPtrInst*, llvm::StoreInst*,
                  llvm::StringRef FieldName);

  //! Where parameters can be added to a function's parameter list.
  enum class ParamPosition { Beginning, End };

  /**
   * Adapt a function call to use a function with an extended API.
   *
   * This is helpful for converting code that uses an existing API to use
   * an extended version of the same function with additional metadata.
   * This is an alternative to run-time linker tricks such as LD_PRELOAD.
   *
   * This method can only be used to extend function calls by adding additional
   * arguments at the beginning or the end of the parameter list: all of the
   * original arguments will still be passed and the return value will not
   * be modified.
   *
   * @param   Call        the original function call: will be replaced with a
   *                      call to the new, extended function
   * @param   NewName     the name of the new, extended function
   * @param   NewArgs     the arguments to be added to the call
   * @param   Position    where the new parameters should be added
   */
  bool Extend(llvm::CallInst *Call, llvm::StringRef NewName,
              llvm::ArrayRef<llvm::Value*> NewArgs,
              ParamPosition Position = ParamPosition::End);


private:
  Instrumenter(llvm::Module&, NameFn NF, std::unique_ptr<InstrStrategy>);

  Instrumentation& GetOrCreateInstr(llvm::StringRef Name,
                                    llvm::StringRef FormatStringPrefix,
                                    const ParamVec&);

  uint32_t FieldNumber(llvm::GetElementPtrInst*);

  llvm::Module& Mod;
  std::unique_ptr<InstrStrategy> Strategy;
  llvm::StringMap<std::unique_ptr<Instrumentation>> Instr;
  NameFn Name;
};

} // namespace loom

#endif // LOOM_INSTRUMENTER_H_
