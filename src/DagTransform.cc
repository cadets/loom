//! @file DagTransform.cc  Definition of @ref DagTransform.
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

#include "DagTransform.hh"

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"
#include "llvm/IR/Module.h"
#include <queue>

using namespace loom;
using namespace llvm;
using std::vector;
using std::queue;


bool loom::findAllUsers(Instruction *Call, 
		std::queue<StringRef> dagTail, 
		std::vector<Instruction*>& toDelete) {
	

	bool isDagComplete = false;
	StringRef nextCall;
	if (dagTail.empty()) {
		isDagComplete = true;
	}

	for (User *U: Call->users())
	{
		if (Instruction *Inst = dyn_cast<Instruction>(U)) {
			toDelete.push_back(Inst);
			if (CallInst *Call = dyn_cast<CallInst>(Inst)) {
				Function *Target = Call->getCalledFunction();
				if (Target->getName() == dagTail.front()) {
					dagTail.pop();
				}
			}
			isDagComplete |= findAllUsers(Inst, dagTail, toDelete);
			if (StoreInst *Store = dyn_cast<StoreInst>(Inst)) {
				isDagComplete |= findStoreCalls(Store, dagTail, toDelete);
			}
	
		}
	}
	return isDagComplete;
}

bool loom::findStoreCalls(StoreInst *Store, 
		std::queue<StringRef> dagTail, 
		std::vector<Instruction*>& toDelete) {

	bool isDagComplete = false;

	for (User* U : Store->getPointerOperand()->users()) {
		if (Instruction *Inst = dyn_cast<Instruction>(U)) {
			for (User *User: Inst->users()) {
				if (CallInst *Call = dyn_cast<CallInst>(User)) {
					Function *Target = Call->getCalledFunction();
					if (Target->getName() == dagTail.front()) {
						dagTail.pop();
						toDelete.push_back(Call);
						isDagComplete |= findAllUsers(Call, dagTail, toDelete);
					}
				}
			}
		}
	}

	return isDagComplete;
}
