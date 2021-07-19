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


bool loom::findAllUsers(Instruction *Inst, 
		std::queue<StringRef> dagTail, 
		//std::vector<Instruction*>& toDelete,
		std::set<Instruction*>& toDelete,
		std::vector<Value*>& arguments) {
	
	if (CallInst* c = dyn_cast<CallInst>(Inst)) {
		StringRef calledFn = c->getCalledFunction()->getName();
		if (calledFn == "socket") {
			arguments.push_back(c->getArgOperand(0));
			arguments.push_back(c->getArgOperand(1));
			arguments.push_back(c->getArgOperand(2));
		} else if (calledFn == "__inet_pton") {
			arguments.push_back(c->getArgOperand(1));
		}
	}

	bool isDagComplete = false;
	if (dagTail.empty()) {
		isDagComplete = true;
	}

	for (User *U: Inst->users())
	{
		if (Instruction *I = dyn_cast<Instruction>(U)) {
			toDelete.insert(I); //push_back(I);
			/* if (CallInst *Call = dyn_cast<CallInst>(I)) { */
			/* 	Function *Target = Call->getCalledFunction(); */
			/* 	if (Target->getName() == dagTail.front()) { */
			/* 		dagTail.pop(); */
			/* 	} */
			/* } */
			if (StoreInst *Store = dyn_cast<StoreInst>(I)) {
				isDagComplete |= findStoreCalls(Store, dagTail, toDelete, arguments);
			} else {
				isDagComplete |= findAllUsers(I, dagTail, toDelete, arguments);
			}
			/* isDagComplete |= findAllUsers(I, dagTail, toDelete, arguments); */
			/* if (StoreInst *Store = dyn_cast<StoreInst>(I)) { */
			/* 	isDagComplete |= findStoreCalls(Store, dagTail, toDelete, arguments); */
			/* } */
	
		}
	}
	return isDagComplete;
}

bool loom::findStoreCalls(StoreInst *Store, 
		std::queue<StringRef> dagTail, 
		// std::vector<Instruction*>& toDelete,
		std::set<Instruction*>& toDelete,
		std::vector<Value*>& arguments) {

	bool isDagComplete = false;

	for (User* U : Store->getPointerOperand()->users()) {
		if (Instruction *Inst = dyn_cast<Instruction>(U)) {
			toDelete.insert(Inst);//push_back(Inst);
			for (User *U: Inst->users()) {
				/* if (CallInst *Call = dyn_cast<CallInst>(U)) { */
				/* 	Function *Target = Call->getCalledFunction(); */
				/* 	if (Target->getName() == dagTail.front()) { */
				/* 		dagTail.pop(); */
				/* 	} */
				/* } */
				
				if (Instruction *I = dyn_cast<Instruction>(U)) {
					toDelete.insert(I);
					isDagComplete |= findAllUsers(I, dagTail, toDelete, arguments);
				}

			}
		}
	}

	return isDagComplete;
}
