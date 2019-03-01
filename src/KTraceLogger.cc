//! @file KTraceLogger.cc  Definition of @ref loom::KTraceLogger.
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

#include "KTraceLogger.hh"
#include "Serializer.hh"

#include <llvm/IR/Module.h>
#include <llvm/IR/TypeBuilder.h>

using namespace loom;
using namespace llvm;
using std::vector;

KTraceLogger::KTraceLogger(Module &Mod, std::unique_ptr<Serializer> S, bool K)
    : Logger(Mod), Serial(std::move(S)), KernelMode(K) {
  assert(Serial && "no Serializer passed into KTraceLogger");
}

Value *KTraceLogger::Log(Instruction *I, ArrayRef<Value *> Values,
                         StringRef Name, StringRef Descrip, loom::Metadata Md,
                         std::vector<loom::Transform> Transforms, bool /* SuppressUniqueness */) {

  IRBuilder<> B(I);

  // Get pointer to serialized data buffer:
  Serializer::BufferInfo Buffer = Serial->Serialize(Name, Descrip, Values, B);

  LLVMContext &Ctx = Mod.getContext();

  if (!KernelMode) {
    // Send record to `utrace`:
    auto *FT = TypeBuilder<int(const void *, size_t), false>::get(Ctx);
    Constant *F = Mod.getOrInsertFunction("utrace", FT);

    B.CreateCall(F, {Buffer.first, Buffer.second});

  } else {
    // Send record to `ktrstruct`:
    auto *FT = TypeBuilder<void(const char *, void *, size_t), false>::get(Ctx);
    Constant *F = Mod.getOrInsertFunction("ktrstruct", FT);
    Value *Name = B.CreateGlobalStringPtr(Serial->SchemeName(), "scheme");

    B.CreateCall(F, {Name, Buffer.first, Buffer.second});
  }

  return Serial->Cleanup(Buffer, B);
}
