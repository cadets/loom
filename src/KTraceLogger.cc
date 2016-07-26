//! @file KTraceLogger.cc  Definition of @ref KTraceLogger.
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

namespace {

} // anonymous namespace


KTraceLogger::KTraceLogger(Module& Mod, std::unique_ptr<Serializer> S, bool K)
  : Logger(Mod), Serial(std::move(S)), KernelMode(K)
{
  assert(Serial && "no Serializer passed into KTraceLogger");
}


void KTraceLogger::Log(IRBuilder<>& B, ArrayRef<Value*> Values,
                       StringRef Name, StringRef Descrip) {

  // Get pointer to serialized data buffer:
  Serializer::BufferInfo Buffer = Serial->Serialize(Name, Descrip, Values, B);

  if (!KernelMode) {
    // User-mode `utrace()` is simple:
    vector<Value*> Args { Buffer.first, Buffer.second };
    B.CreateCall(GetUTrace(), Args);

  } else {
  }

  Serial->Cleanup(Buffer, B);
}


Function* KTraceLogger::GetUTrace() {
  LLVMContext &Ctx = Mod.getContext();
  FunctionType *FT = TypeBuilder<int(const void*, size_t), false>::get(Ctx);

  Function *F = dyn_cast<Function>(Mod.getOrInsertFunction("utrace", FT));
  assert(F);

  return F;
}
