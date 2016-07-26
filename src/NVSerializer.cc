//! @file NVSerializer.cc  Definition of @ref NVSerializer.
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

#include "NVSerializer.hh"

#include <llvm/IR/Module.h>
#include <llvm/IR/TypeBuilder.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;
using namespace loom;

namespace {

cl::opt<bool>
NVDebug("loom-nv-debug", cl::desc("emit libnv debug code"), cl::init(false));

} // anonymous namespace


namespace loom {

/// Object that finds/creates libnv functions.
class LibNV {
public:
  LibNV(Module&);

  CallInst* Free(Value*, IRBuilder<>&);         //!< libc's free(void*)

  CallInst* Create(IRBuilder<>&);               //!< _create(int flags = 0)
  CallInst* Destroy(Value*, IRBuilder<>&);      //!< _destroy(nvlist_t*)
  CallInst* Pack(Value*, Value*, IRBuilder<>&); //!< _pack(nvlist_t*, size_t*)
  CallInst* Dump(Value*, Value*, IRBuilder<>&); //!< _dump(nvlist_t*, int fd)

  /// Add a named value to the NVList.
  void Add(Value *List, StringRef Name, Value *V, IRBuilder<>&);

  /**
   * Add a static string to the NVList.
   *
   * For the moment, this is the only way to serialize a string with LibNV:
   * we can only trust that a string is a valid C-style string if we were able
   * to inspect it at instrumentation time.
   */
  void Add(Value *List, StringRef Name, StringRef Value, IRBuilder<>&);

  Constant* Fn(StringRef Name, Type* Ret, ArrayRef<Type*> Params);

  Module &M;
  LLVMContext &Ctx;
  Type *Void;
  IntegerType *Int;
  IntegerType *SizeT;
  PointerType *BytePtr;
  PointerType *NVListPtr;
};

} // loom namespace


NVSerializer::NVSerializer(llvm::Module &M)
  : Serializer(M.getContext()), NV(new LibNV(M))
{
}


Serializer::BufferInfo
NVSerializer::Serialize(ArrayRef<Value*> Values, IRBuilder<>& B) {
  Value *NVList = NV->Create(B);

  for (Value *V : Values) {
    NV->Add(NVList, V->getName(), V, B);
  }

  if (NVDebug) {
    NV->Dump(NVList, ConstantInt::get(NV->Int, 2 /* stderr */), B);
  }

  Value *SizePtr = B.CreateAlloca(SizeT);
  Value *Buffer = NV->Pack(NVList, SizePtr, B);

  // Destroy the nvlist_t: all we need now is the packed buffer
  NV->Destroy(NVList, B);

  return { Buffer, B.CreateLoad(SizePtr) };
}


void NVSerializer::Cleanup(BufferInfo& Buffer, IRBuilder<>& B) {
  NV->Free(Buffer.first, B);
}


LibNV::LibNV(Module& M)
  : M(M), Ctx(M.getContext()),
    Void(Type::getVoidTy(Ctx)),
    Int(IntegerType::get(Ctx, 32)),
    SizeT(TypeBuilder<size_t, false>::get(Ctx)),
    BytePtr(PointerType::getUnqual(IntegerType::get(Ctx, 8))),
    NVListPtr(PointerType::getUnqual(StructType::create(Ctx, "nvlist_t")))
{
}


CallInst* LibNV::Free(Value *V, IRBuilder<>& B) {
  assert(V->getType() == BytePtr);
  Constant *F = Fn("free", Void, { BytePtr });
  return B.CreateCall(F, V);
}

CallInst* LibNV::Create(IRBuilder<>& B) {
  Constant *F = Fn("nvlist_create", NVListPtr, { Int });

  ConstantInt *Zero = ConstantInt::get(Int, 0);
  return B.CreateCall(F, Zero);
}

CallInst* LibNV::Destroy(Value *NVList, IRBuilder<>& B) {
  assert(NVList->getType() == NVListPtr);
  Constant *F = Fn("nvlist_destroy", Void, { NVListPtr });

  return B.CreateCall(F, NVList);
}

CallInst* LibNV::Pack(Value *NVList, Value *SizePtr, IRBuilder<>& B) {
  assert(NVList->getType() == NVListPtr);
  PointerType *SizeTPtr = PointerType::getUnqual(SizeT);
  Constant *F = Fn("nvlist_pack", BytePtr, { NVListPtr, SizeTPtr });

  return B.CreateCall(F, { NVList, SizePtr });
}

CallInst* LibNV::Dump(Value *NVList, Value *File, IRBuilder<>& B) {
  assert(NVList->getType() == NVListPtr);
  Constant *F = Fn("nvlist_dump", BytePtr, { NVListPtr, Int });

  return B.CreateCall(F, { NVList, File });
}

void LibNV::Add(Value *List, StringRef Name, Value *V, IRBuilder<>& B) {
  Type *T = V->getType();
  Constant *F = nullptr;

  if (T->isFloatTy() or T->isDoubleTy()) {
    llvm::errs() << "WARNING: libnv doesn't support floating-point numbers\n";

  } else if (auto *IT = dyn_cast<IntegerType>(T)) {
    if (IT->getBitWidth() == 1) {
      IntegerType *Bool = IntegerType::get(Ctx, 1);
      F = Fn("nvlist_add_bool", Void, { NVListPtr, BytePtr, Bool });

    } else {
      F = Fn("nvlist_add_number", Void, { NVListPtr, BytePtr, SizeT });

      if (T != SizeT) {
        V = B.CreateSExt(V, SizeT);
      }
    }

  } else if (T->isPointerTy()) {
    F = Fn("nvlist_add_number", Void, { NVListPtr, BytePtr, SizeT });
    V = B.CreatePointerCast(V, SizeT);

  } else {
    raw_ostream& err = llvm::errs();
    err << "WARNING: NVSerializer doesn't support ";
    T->print(err, true);
    err << " (yet)\n";
  }

  if (F == nullptr)
    return;

  Value *NameVal = B.CreateGlobalStringPtr(Name);

  B.CreateCall(F, { List, NameVal, V });
}

void LibNV::Add(Value *List, StringRef Name, StringRef Str, IRBuilder<>& B) {
  Constant *F = Fn("nvlist_add_string", Void, { NVListPtr, BytePtr, BytePtr });

  Value *NamePtr = B.CreateGlobalStringPtr(Name);
  Value *V = B.CreateGlobalStringPtr(Str);

  B.CreateCall(F, { List, NamePtr, V });
}

Constant* LibNV::Fn(StringRef Name, Type* Ret, ArrayRef<Type*> Params) {
  FunctionType *T = FunctionType::get(Ret, Params, false);
  return M.getOrInsertFunction(Name, T);
}
