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

#include "InstrumentationFn.hh"
#include "Logger.hh"
#include "Policy.hh"

#include <functional>


namespace loom {

/// An object used for instrumenting code within a single LLVM module.
class Instrumenter {
public:
  typedef std::function<std::string (const std::vector<std::string>&)> NameFn;

  /**
   * Create a new Instrumenter instance.
   *
   * @param   NF      a function that can be called to name instrumentation
   *                  (e.g., ["call","foo"] => "__loom_hook_call_foo")
   */
  static std::unique_ptr<Instrumenter>
    Create(llvm::Module&, NameFn NF, Logger::LogType L = Logger::LogType::None);

  /// Instrument a function call in the call and/or return direction.
  bool Instrument(llvm::CallInst*, const std::vector<Policy::Direction>&);

  /// Instrument a function call (caller-side), either calling or returning.
  bool Instrument(llvm::CallInst *Call, Policy::Direction);

  /// Instrument a function entry and/or exit.
  bool Instrument(llvm::Function&, const std::vector<Policy::Direction>&);

  /// Instrument a function entry or exit.
  bool Instrument(llvm::Function&, Policy::Direction);


private:
  Instrumenter(llvm::Module&, NameFn NF, std::unique_ptr<Logger>);

  InstrumentationFn& GetOrCreateInstrFn(llvm::StringRef Name,
                                        llvm::StringRef FormatStringPrefix,
                                        const ParamVec&,
                                        llvm::GlobalValue::LinkageTypes,
                                        bool CreateDefinition);

  llvm::Module& Mod;
  llvm::StringMap<std::unique_ptr<InstrumentationFn>> InstrFns;
  NameFn Name;
  std::unique_ptr<Logger> Log;
};

} // namespace loom

#endif // LOOM_INSTRUMENTER_H_
