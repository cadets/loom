//! @file DTraceLogger.hh  Declaration of @refDagTransform.
/*
 * Copyright (c) 2021 Brian Kidney
 * All rights reserved.
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

#ifndef DAG_TRANSFORM_H_
#define DAG_TRANSFORM_H_

#include <vector>
#include <queue>
#include <set>

namespace llvm {
class BasicBlock;
class Value;
class Instruction;
class StoreInst;
class StringRef;
} // namespace llvm


namespace loom {

	/* bool findAllUsers(llvm::Instruction *Call, std::queue<llvm::StringRef> dagTail, std::vector<llvm::Instruction*>& toDelete, std::vector<llvm::Value*>& arguments); */
	/* bool findStoreCalls(llvm::StoreInst *Store, std::queue<llvm::StringRef> dagTail, std::vector<llvm::Instruction*>& toDelete, std::vector<llvm::Value*>& arguments); */
	bool findAllUsers(llvm::Instruction *Call, std::queue<llvm::StringRef> dagTail, std::set<llvm::Instruction*>& toDelete, std::vector<llvm::Value*>& arguments);
	bool findStoreCalls(llvm::StoreInst *Store, std::queue<llvm::StringRef> dagTail, std::set<llvm::Instruction*>& toDelete, std::vector<llvm::Value*>& arguments);

} // namespace loom

#endif // !DAG_TRANSFORM_H_
