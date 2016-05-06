//! @file IRUtils.h  Declarations of LLVM IR utility functions.
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

#ifndef LOOM_IRUTILS_H
#define LOOM_IRUTILS_H

#include <llvm/ADT/StringRef.h>
#include <llvm/IR/IRBuilder.h>

#include <vector>


namespace loom {

/// Types of format strings that Loom knows how to generate.
enum class FormatStringStyle {

  /// Conventional printf() formatting.
  Printf,

  /// Extended formatting information to pass to libxo's xo_emit.
  Xo,
};

/// Information about a (named) parameter to a function.
typedef std::pair<std::string,llvm::Type*> Parameter;

/**
 * Create a format string for a call to printf (or a printf-like function),
 * store it in a module as a global variable and get a pointer to it, suitable
 * for passing directly to printf.
 *
 * @param    Prefix      text to print before the values
 * @param    Values      names and types for the values that will be passed
 *                       to printf together with the format string,
 *                       and which the format string should describe
 * @param    Suffix      text to print after the values
 * @param    Style       what sort of format string to generate
 *                       (traditional printf, libxo, etc.)
 */
llvm::Value*
CreateFormatString(llvm::Module&, llvm::IRBuilder<>&,
                   llvm::StringRef Prefix, llvm::ArrayRef<Parameter> Values,
                   llvm::StringRef Suffix = "",
                   FormatStringStyle Style = FormatStringStyle::Printf);

/// Find a named BasicBlock within a function.
llvm::BasicBlock* FindBlock(llvm::StringRef Name, llvm::Function&);

/// Retrieve a function's parameter names and types.
std::vector<Parameter> GetParameters(llvm::Function*);

/// Find or declare a printf-like function, i.e., `int foobar(const char*, ...)`
llvm::Function* GetPrintfLikeFunction(llvm::Module&,
                                             llvm::StringRef Name = "printf");

} // namespace loom

#endif // LOOM_IRUTILS_H
