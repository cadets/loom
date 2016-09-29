//! @file Serializer.hh  Declaration of @ref Serializer.
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

#ifndef SERIALIZER_H_
#define SERIALIZER_H_

#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/IRBuilder.h>

namespace loom {

//! A tool that can transform a set of values into a serialized representation.
class Serializer {
public:
  //! A buffer can be described using a pointer and a length.
  typedef std::pair<llvm::Value*, llvm::Value*> BufferInfo;

  //! Create an empty Serializer unique_ptr.
  static std::unique_ptr<Serializer> None();

  //! A unique name that identifies the serialization scheme (e.g., `nvlist`).
  virtual llvm::StringRef SchemeName() const = 0;

  /**
   * Generate the code to serialize a set of values, returning details of the
   * resulting buffer (pointer and length).
   *
   * @param  Name         Machine-readable instrumentation name.
   * @param  Description  Human-readable short description (may be ignored).
   * @param  Values       Values actually being serialized
   * @param  B            IRBuilder positioned within instrumentation code
   */
  virtual BufferInfo Serialize(llvm::StringRef Name, llvm::StringRef Descrip,
                               llvm::ArrayRef<llvm::Value*> Values,
                               llvm::IRBuilder<>& B) = 0;

  /**
   * Clean up a serialized data buffer using Serializer-specific cleanup code
   * (e.g., `free()` or `nvlist_destroy`).
   */
  virtual llvm::Value* Cleanup(BufferInfo&, llvm::IRBuilder<>&) = 0;

protected:
  Serializer(llvm::LLVMContext &Ctx);

  llvm::LLVMContext& Ctx;
  llvm::IntegerType *Byte;
  llvm::PointerType *BytePtr;
  llvm::IntegerType *SizeT;
};


//! A Serializer that doesn't serialize anything.
class NullSerializer : public Serializer {
public:
  NullSerializer(llvm::LLVMContext &Ctx) : Serializer(Ctx) {}

  virtual llvm::StringRef SchemeName() const override { return "null"; }

  virtual BufferInfo Serialize(llvm::StringRef Name, llvm::StringRef Descrip,
                               llvm::ArrayRef<llvm::Value*>,
                               llvm::IRBuilder<>&) override;

  virtual llvm::Value* Cleanup(BufferInfo&, llvm::IRBuilder<>&) override;

private:
  llvm::Value* Nop(llvm::IRBuilder<>&);
};

} // namespace loom

#endif // !SERIALIZER_H_
