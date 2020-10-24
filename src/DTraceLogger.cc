//! @file DTraceLogger.cc  Definition of @ref DTraceLogger.
/*
 * Copyright (c) 2017 Brian Kidney
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

#include "DTraceLogger.hh"

#include <algorithm>

#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

using namespace loom;
using namespace llvm;
using std::vector;

DTraceLogger::DTraceLogger(llvm::Module& Mod)
  : Logger(Mod){ };

Value* DTraceLogger::ConvertValueToPtr(IRBuilder<>& B, LLVMContext& Ctx, Value* V, Type* param_t)
{
	Type *T = V->getType();
	if (T->isPointerTy()) {
		return  B.CreatePtrToInt(V, param_t);
	} else if (T->isIntegerTy()) {
		return B.CreateSExt(V, param_t);
	} else if (T->isDoubleTy()) {
		return B.CreateBitCast(V, IntegerType::get(Ctx, sizeof(int64_t) * CHAR_BIT));
	} else if (T->isFloatTy()) {
		auto *BC = B.CreateBitCast(V, IntegerType::get(Ctx, sizeof(int32_t) * CHAR_BIT));
		return B.CreateSExt(BC, param_t);
	}
	return ConstantInt::get(param_t, 0);
}



Value* DTraceLogger::Log(Instruction *I, ArrayRef<Value*> Values,
                         StringRef Name, StringRef Descrip,
                         Metadata Metadata, std::vector<Transform> Transforms,
                         bool /* SuppressUniqueness */) {

  if (not Metadata.isValid())
	  errs() << "Warning: Dtrace Logger requires the use of Metadata to function properly.";

  IRBuilder<> B(I);

  LLVMContext &Ctx = Mod.getContext();
  Type* id_t = IntegerType::get(Ctx, sizeof(unsigned int) * CHAR_BIT);
  Type* param_t = IntegerType::get(Ctx, sizeof(uintptr_t) * CHAR_BIT);

  size_t n_args = std::min(Values.size(), 5ul);

  std::vector<Type*> params;
  for (unsigned int i = 0; i < n_args + 1; i++) {
      params.push_back(param_t);
  }
  
  auto *FT = FunctionType::get( IntegerType::get(Ctx, sizeof(int) * CHAR_BIT), params, false);
  
  Constant *F = Mod.getOrInsertFunction("dt_probe", FT);


  Value* args[6];
  args[0] = B.CreateSExt(ConstantInt::get(id_t, Metadata.Id), param_t);;
  for (unsigned int i = 0; i < n_args; i++)
  {
    Value *ptr;

	Transform Tf;
	for (auto t: Transforms) {
		if (t.Arg == i)
			Tf = t;
			break;
	}

	if (Tf.isValid()) {
		ptr = ConvertValueToPtr(B, Ctx, Tf.CreateTransform(I, Mod, Values[i]), param_t);
	} else {
		ptr = ConvertValueToPtr(B, Ctx, Values[i], param_t);
	}
	
    args[i + 1] = ptr;
  }

  ArrayRef<Value*> argsRef(args, n_args + 1);

  return B.CreateCall(F, argsRef);

}

