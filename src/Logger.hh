//! @file Logger.hh  Declaration of @ref loom::Logger.
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

#ifndef LOOM_LOGGER_H
#define LOOM_LOGGER_H

#include "IRUtils.hh"

#include <llvm/IR/IRBuilder.h>

#include <memory>

namespace llvm {
  class Function;
  class FunctionType;
  class Module;
  class Value;
}

namespace loom {

/// Something that can be used to log events, e.g., `printf` or `libxo`.
class Logger {
public:
  /// Ways that we can log values.
  enum class LogType {

    /// The libc printf() function
    Printf,

    /// Juniper's libxo, which generates text or structured output
    Libxo,

    /// Don't log things.
    None,
  };

  /// Create a new Logger of the specified type (`printf`, `libxo`, etc.).
  static std::unique_ptr<Logger> Create(llvm::Module&,
                                        LogType Log = LogType::Printf);

  /// Get (or create) declaration for the logging function.
  llvm::Function* GetFunction();

  /// Get the type of the logging function, often `int (const char*, ...)`.
  virtual llvm::FunctionType* GetType();

  /// Create a format string that we can pass to the logging function.
  virtual llvm::Value* CreateFormatString(llvm::IRBuilder<>&,
                                          llvm::StringRef Prefix,
                                          llvm::ArrayRef<Parameter>,
                                          llvm::StringRef Suffix) = 0;

  /// Create a format string that we can pass to the logging function.
  llvm::Value* CreateFormatString(llvm::IRBuilder<>&, llvm::StringRef Prefix,
                                  llvm::ArrayRef<llvm::Value*>,
                                  llvm::StringRef Suffix);

  /**
   * Adapt a set of values into a form that can be logged.
   *
   * Before values can be passed to a logging function, they may need to be
   * adapted to a suitable form. For example, `float` values may need to be
   * extended into `double` values or aggregates (e.g., `struct` values)
   * may need to be expanded into their constituent parts.
   *
   * This function adapts values as necessary and returns a possibly-larger
   * vector of possibly-adapted values. In the best case, nothing requires
   * adaptation, the IRBuilder is not used and the original values are returned.
   * In the worse case, everything requires adaptation and all of the values in
   * the returned vector will be new, derived from the originals using the
   * supplied IRBuilder.
   *
   * @returns  a vector of possibly-adapted values
   */
  virtual std::vector<llvm::Value*> AdaptArguments(llvm::ArrayRef<llvm::Value*>,
                                                   llvm::IRBuilder<>&);

protected:
  Logger(llvm::Module& Mod) : Mod(Mod) {}

  virtual llvm::StringRef FunctionName() const = 0;

  llvm::Module& Mod;
};

} // namespace loom

#endif // !LOOM_LOGGER_H
