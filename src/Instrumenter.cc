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


Instrumenter::Instrumenter(llvm::Module& Mod, NameFn NF)
  : Mod(Mod), Name(NF)
{
}


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

  vector<Value*> Arguments;
  for (Use *m = Call->arg_begin(), *n = Call->arg_end(); m != n; ++m) {
      Arguments.push_back(m->get());
  }

  const string TargetName = Target->getName();
  Type *CallType = Call->getType();
  const bool voidFunction = CallType->isVoidTy();
  const bool Return = (Dir == Policy::Direction::Out);
  const string Description = Return ? "return" : "call";

  vector<string> InstrNameComponents;
  ParamVec Parameters = GetParameters(Target);

  if (Return and not voidFunction)
      Parameters.emplace(Parameters.begin(), "retval", Call->getType());

  InstrNameComponents.push_back(Description);
  InstrNameComponents.push_back(TargetName);
  const string InstrName = Name(InstrNameComponents);

  // Call instrumentation can be done entirely within a translation unit:
  // calls in other units can use their own instrumentation functions.
  InstrumentationFn& InstrFn = GetOrCreateInstrFn(InstrName, Parameters,
                                                  Function::InternalLinkage,
                                                  true);

  IRBuilder<> Builder = InstrFn.GetPreambleBuilder();
  std::unique_ptr<Logger> Log(Logger::Create(Mod, Logger::LogType::Printf));

  string FormatStringPrefix = Description + " " + TargetName + ":";
  Log->Call(Builder, FormatStringPrefix, InstrFn.GetParameters(), "\n");

  CallInst *InstrCall;

  if (Return) {
    if (not voidFunction)
      Arguments.insert(Arguments.begin(), Call);
    InstrCall = InstrFn.CallAfter(Call, Arguments);
  } else {
    InstrCall = InstrFn.CallBefore(Call, Arguments);
  }

  InstrCall->setAttributes(Target->getAttributes());

  return true;
}


InstrumentationFn&
Instrumenter::GetOrCreateInstrFn(StringRef Name, const ParamVec& P,
                                 GlobalValue::LinkageTypes Linkage,
                                 bool CreateDefinition)
{
  auto i = InstrFns.find(Name);
  if (i != InstrFns.end())
    return *i->second;

  InstrFns[Name] = InstrumentationFn::Create(Name, P, Linkage, Mod,
                                             CreateDefinition);
  return *InstrFns[Name];
}
