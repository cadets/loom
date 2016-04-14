//! @file OptPass.cc  Definition of @ref loom::OptPass.
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


#include "llvm/Pass.h"

#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include <llvm-c/Core.h>
#include "PolicyFile.hh"


using namespace llvm;
using namespace loom;
Function* createInstrFunction(Module &module, CallInst* callInst, Policy::Direction direction, Policy& policy);
std::vector<Type*> getParameterTypes(Function *pOldF);
void setArgumentNames(Function *pOldF, Function *pNewF);
void addPrintfCall(IRBuilder<> Builder, Function *pOldF, Module &module, std::vector<Value*> argumentValues);

namespace {
  struct OptPass : public ModulePass {
    static char ID;
    OptPass() : ModulePass(ID) {}

    bool runOnModule(Module&) override;
  };
}

bool OptPass::runOnModule(Module &module)
{
  ErrorOr<std::unique_ptr<PolicyFile>> policyFile = PolicyFile::Open();
  if (std::error_code err = policyFile.getError())
  {
    errs() << "Error opening LOOM policy file: " << err.message() << "\n";
    return false;
  }

  outs() << "Got policy file!\n";

	std::map<CallInst*, std::vector<Policy::Direction>> callInstsMap;
		ErrorOr<std::unique_ptr<PolicyFile>> p = PolicyFile::Open();
		if (p.getError()) {
			errs() << p.getError().message() << "\n";
			errs() << "error when retrieving Policy File \n";
			return false;
		}
		assert(*p);
		Policy& policy = **p;

    for (Module::iterator i = module.begin(), e = module.end(); i!=e; i++) {
			for (inst_iterator j = inst_begin(*i), e = inst_end(*i); j != e; ++j) {
				if (CallInst* callInst = dyn_cast<CallInst>(&*j)) {  
					auto directions = policy.CallInstrumentation(*callInst->getCalledFunction());
					if (not directions.empty()) {
						callInstsMap.insert(std::make_pair(callInst, directions));	
					}
				}
			}
    }
		//Iterate over call instructions and create a new instrumented function 
		//and create a function call to that new function
		for (std::map<CallInst*, std::vector<Policy::Direction>>::iterator i = callInstsMap.begin(); i != callInstsMap.end(); ++i) {
			
			CallInst* callInst = (*i).first;
			Function* pOldF = callInst->getCalledFunction();
  		callInst->setCallingConv(pOldF->getCallingConv());
			std::vector<Value*> argumentValues;
			for (Use *m = callInst->arg_begin(), *n = callInst->arg_end(); m != n; ++m) {
				  argumentValues.push_back(m->get());
			}
				
			std::vector<Policy::Direction> directions = (*i).second;
			for(std::vector<Policy::Direction>::iterator it=directions.begin(); it !=directions.end(); ++it) {
				Function* pNewF = createInstrFunction(module, (*i).first, *it, policy);
				IRBuilder<> Builder(&*callInst);
				CallInst *callToInstr = Builder.CreateCall(pNewF, argumentValues);		
				callToInstr->setAttributes(pOldF->getAttributes());
			}
		}
    return true;
}

char OptPass::ID = 0;
static RegisterPass<OptPass> X("loom", "Loom instrumentation", false, false);

Function* createInstrFunction(Module &module, CallInst* callInst, Policy::Direction direction, Policy& policy) {
	Function* pOldF = callInst->getCalledFunction();
 	callInst->setCallingConv(pOldF->getCallingConv());
	std::vector<Value*> argumentValues;
	for (Use *m = callInst->arg_begin(), *n = callInst->arg_end(); m != n; ++m) {
		  argumentValues.push_back(m->get());
	}
	std::string oldName = pOldF->getName();
	std::string newFunctionName;
	std::vector<std::string> instrNameArgs;
	std::string arg = "";
	if (direction == Policy::Direction::In) {
		arg = "call";
	}
	else if (direction == Policy::Direction::Out) {
		arg = "return";
	}
	instrNameArgs.push_back(arg);
	arg = callInst->getCalledFunction()->getName();
	instrNameArgs.push_back(arg);
	arg = "";
	instrNameArgs.push_back(arg);
	newFunctionName = policy.InstrName(instrNameArgs);
	
  FunctionType *pNewFT = FunctionType::get(Type::getVoidTy(module.getContext()), getParameterTypes(pOldF), false);
	Function *pNewF = Function::Create(pNewFT, Function::ExternalLinkage, newFunctionName, &module);
  pNewF->setCallingConv(pOldF->getCallingConv());
	setArgumentNames(pOldF, pNewF);
	//Create a basic block and add printf call
  BasicBlock *block = BasicBlock::Create(module.getContext(), "entry", pNewF);
	IRBuilder<> Builder(block);
	addPrintfCall(Builder, pOldF, module, argumentValues);
	Builder.CreateRetVoid();
	return pNewF;
}

std::vector<Type*> getParameterTypes(Function *pOldF) {
	std::vector<Type*> paramTypes;
	SymbolTableList<Argument> *oldArgList = &(pOldF->getArgumentList()); 
	for(ilist_iterator<Argument> k = oldArgList->begin(), l = oldArgList->end(); k != l; k++) {
		paramTypes.push_back(k->getType());
	}
	return paramTypes;
}

void setArgumentNames(Function *pOldF, Function *pNewF) {
	SymbolTableList<Argument> *oldArgList = &(pOldF->getArgumentList()); 
	for(ilist_iterator<Argument> k = oldArgList->begin(), l = oldArgList->end(), m = pNewF->getArgumentList().begin(); k != l; ++k, ++m) {
		m->setName(k->getName());
	}
}

void addPrintfCall(IRBuilder<> Builder, Function *pOldF, Module &module, std::vector<Value*> argumentValues) {
	FunctionType *printfFT = TypeBuilder<int(const char *, ...), false>::get(pOldF->getContext());	
	Function *printf = Function::Create(printfFT, Function::ExternalLinkage, "printf", &module);
  //printf->setCallingConv(pOldF->getCallingConv());
	std::vector<Value*> printfArgs;
	std::string calledFunctionName = pOldF->getName();
	std::string printfStringArg = "calling " + calledFunctionName + "(";
	for(std::vector<Value*>::iterator it=argumentValues.begin(); it !=argumentValues.end(); ++it) {
		Type *argType = (*it)->getType();
		if(argType->isIntegerTy(32)) {
			printfStringArg.append("%d");
		}
		else if (argType->isFloatTy() || argType->isDoubleTy()) {
			printfStringArg.append("%.0f");
		}
		else if (argType->isIntegerTy(8)) {
			printfStringArg.append("%c");
		}
		else if (argType->isPointerTy()) {
			printfStringArg.append("%s");
		}
		if(*it==argumentValues.back()) {
			printfStringArg.append(")\n\"");
		}
		else {
			printfStringArg.append(", ");
		}
	}
	Value *arg1 = Builder.CreateGlobalStringPtr(printfStringArg);
	printfArgs.push_back(arg1);
	printfArgs.insert(printfArgs.end(), argumentValues.begin(), argumentValues.end());
	Builder.CreateCall(printf, printfArgs);
}
