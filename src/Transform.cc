//! @file Transforms.cc  Defines transforms that can be applied to IR Data.
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


Value* Transform::CreateTransform(Instruction* I, Module& Mod, Value* V) {

	Value* call = V;

	if (this->Fn == "GetUUID") {
	  call = CreateUUIDTransform(I, Mod, V);
	} else {
	  errs() << "Warning: Dtrace Logger requires the use of Metadata to function properly.";
	  call = nullptr;
	}

	return call;
}

/* Length of a node address (an IEEE 802 address). */
/* #define _UUID_NODE_LEN      6 */
/* struct uuid { */
/* 	uint32_t    time_low; */
/* 	uint16_t    time_mid; */
/* 	uint16_t    time_hi_and_version; */
/* 	uint8_t     clock_seq_hi_and_reserved; */
/* 	uint8_t     clock_seq_low; */
/* 	uint8_t     node[_UUID_NODE_LEN]; */
/* } */

/* int fgetuuid(int, struct uuid *); */

Value* Transform::CreateUUIDTransform(Instruction* I, Module& Mod, Value* V) {

  IRBuilder<> B(I);

  LLVMContext &Ctx = Mod.getContext();
  Type* fd_t = IntegerType::get(Ctx, sizeof(int) * CHAR_BIT);
  Type* uuid_t = StructType::get( IntegerType::get(Ctx, sizeof(uint32_t) * CHAR_BIT),
								  IntegerType::get(Ctx, sizeof(uint16_t) * CHAR_BIT),
								  IntegerType::get(Ctx, sizeof(uint16_t) * CHAR_BIT),
								  IntegerType::get(Ctx, sizeof(uint8_t) * CHAR_BIT),
								  IntegerType::get(Ctx, sizeof(uint8_t) * CHAR_BIT),
								  ArrayType::get(IntegerType::get(Ctx, sizeof(uint8_t) * CHAR_BIT), 6));


  Value* args[2];
  args[0] = V;
  args[1] = B.CreateAlloca(uuid_t); 
  ArrayRef<Value*> argsRef(args, 2);
  
  std::vector<Type*> params;
  params.push_back(fd_t);
  params.push_back(args[1]->getType());
  
  auto *FT = FunctionType::get( IntegerType::get(Ctx, sizeof(int) * CHAR_BIT), params, false);
  
  Constant *F = Mod.getOrInsertFunction("fgetuuid", FT);


  B.CreateCall(F, argsRef);

  return argsRef.data()[1];

}

