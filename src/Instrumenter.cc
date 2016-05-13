//! @file Instrumenter.cc  Definition of @ref loom::Instrumenter.
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

#include "Instrumenter.hh"
#include "Logger.hh"

#include <sstream>

using namespace llvm;
using namespace loom;
using std::string;
using std::vector;
using std::unique_ptr;


unique_ptr<Instrumenter>
Instrumenter::Create(Module& Mod, NameFn NF, Logger::LogType L) {
  unique_ptr<Logger> Log = Logger::Create(Mod, L);
  return unique_ptr<Instrumenter>(new Instrumenter(Mod, NF, std::move(Log)));
}


Instrumenter::Instrumenter(llvm::Module& Mod, NameFn NF, unique_ptr<Logger> Log)
  : Mod(Mod), Name(NF), Log(std::move(Log)) {}


bool Instrumenter::Instrument(CallInst *Call,
                              const std::vector<Policy::Direction>& D)
{
  bool ModifiedIR = false;

  for(auto Dir : D) {
    ModifiedIR |= Instrument(Call, Dir);
  }

  return ModifiedIR;
}


bool Instrumenter::Instrument(llvm::CallInst *Call, Policy::Direction Dir)
{
  Function* Target = Call->getCalledFunction();
  assert(Target); // TODO: support indirect targets, too

  // Get some relevant details about the call and the target function.
  const string TargetName = Target->getName();
  Type *CallType = Call->getType();
  const bool voidFunction = CallType->isVoidTy();
  const bool Return = (Dir == Policy::Direction::Out);

  //
  // Find or create the instrumentation function.
  //
  ParamVec Parameters = GetParameters(Target);
  if (Return and not voidFunction)
      Parameters.emplace(Parameters.begin(), "retval", Call->getType());

  const string Description = Return ? "return" : "call";
  const string FormatStringPrefix = Description + " " + TargetName + ":";

  vector<string> InstrNameComponents { Description, TargetName };
  const string InstrName = Name(InstrNameComponents);

  // Call instrumentation can be done entirely within a translation unit:
  // calls in other units can use their own instrumentation functions.
  InstrumentationFn& InstrFn = GetOrCreateInstrFn(InstrName,
                                                  FormatStringPrefix,
                                                  Parameters,
                                                  Function::InternalLinkage,
                                                  true);

  //
  // Having found the instrumentation function, call it, passing in
  // the same values passed to the target by the CallInst of interest,
  // augmenting with the returned value if appropriate.
  //
  vector<Value*> Arguments;
  if (Return and not voidFunction) {
    Arguments.push_back(Call);  // the return value, if present, comes first
  }

  for (Use *m = Call->arg_begin(), *n = Call->arg_end(); m != n; ++m) {
    Arguments.push_back(m->get());
  }

  CallInst *InstrCall =
    Return
    ? InstrFn.CallAfter(Call, Arguments)
    : InstrFn.CallBefore(Call, Arguments)
    ;

  InstrCall->setAttributes(Target->getAttributes());

  return true;
}


bool
Instrumenter::Instrument(Function& Fn, const vector<Policy::Direction>& D) {
  bool ModifiedIR = false;

  for (auto Dir : D) {
    ModifiedIR |= Instrument(Fn, Dir);
  }

  return ModifiedIR;
}


bool
Instrumenter::Instrument(Function& Fn, Policy::Direction Dir) {
  const bool Return = (Dir == Policy::Direction::Out);
  const string Description = Return ? "leave" : "enter";
  StringRef FnName = Fn.getName();

  assert(isa<PointerType>(Fn.getType()));
  FunctionType *FnType = dyn_cast<FunctionType>(Fn.getType()->getElementType());
  assert(FnType);
  const bool voidFunction = FnType->isVoidTy();

  vector<string> InstrNameComponents { Description, FnName };
  const string InstrName = Name(InstrNameComponents);

  vector<Value*> Arguments;
  vector<Parameter> InstrParameters;

  if (Return and not voidFunction)
    InstrParameters.emplace_back("retval", FnType->getReturnType());

  for (auto& Arg : Fn.getArgumentList()) {
      Arguments.push_back(&Arg);
      InstrParameters.emplace_back(Arg.getName(), Arg.getType());
  }


  string FormatStringPrefix = (Description + " " + FnName + ":").str();

  // Callee-side function instrumentation can have internal linkage.
  InstrumentationFn& InstrFn = GetOrCreateInstrFn(InstrName,
                                                  FormatStringPrefix,
                                                  InstrParameters,
                                                  Function::InternalLinkage,
                                                  true);

  if (Return) {
    for (auto& Block : Fn) {
      TerminatorInst *Terminator = Block.getTerminator();
      if (auto *Ret = dyn_cast<ReturnInst>(Terminator)) {
        vector<Value*> InstrArgs;
        if (not voidFunction) {
          InstrArgs.push_back(Ret->getReturnValue());
        }
        InstrArgs.insert(InstrArgs.end(), Arguments.begin(), Arguments.end());

        InstrFn.CallBefore(Terminator, InstrArgs);
      }
    }

  } else {
    assert(not Fn.getBasicBlockList().empty());
    BasicBlock& Entry = Fn.getBasicBlockList().front();

    InstrFn.CallBefore(&Entry.front(), Arguments);
  }

  return false;
}


InstrumentationFn&
Instrumenter::GetOrCreateInstrFn(StringRef Name, StringRef FormatPrefix,
                                 const ParamVec& P,
                                 GlobalValue::LinkageTypes Linkage,
                                 bool CreateDefinition)
{
  // Does this function already exist?
  auto i = InstrFns.find(Name);
  if (i != InstrFns.end())
    return *i->second;

  // The instrumentation function doesn't already exist, so create it.
  InstrFns[Name] = InstrumentationFn::Create(Name, P, Linkage, Mod,
                                             CreateDefinition);

  assert(InstrFns[Name]);
  InstrumentationFn& Fn = *InstrFns[Name];

  if (Log) {
    IRBuilder<> Builder = Fn.GetPreambleBuilder();
    Log->Call(Builder, FormatPrefix, Fn.GetParameters(), "\n");
  }

  return Fn;
}
