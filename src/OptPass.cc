//! @file OptPass.cc  Definition of @ref loom::OptPass.
/*
 * Copyright (c) 2016 Jonathan Anderson
 * Copyright (c) 2016 Cem Kilic
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

#include "PolicyFile.hh"
#include "Instrumenter.hh"
#include "IRUtils.hh"

#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Support/raw_ostream.h"

#include <unordered_map>

using namespace llvm;
using namespace loom;
using std::string;
using std::vector;


namespace {
  cl::opt<Logger::LogType> LogType(
    "loom-logging",
    cl::desc("Logging performed automatically by LOOM instrumentation:"),
    cl::values(
      clEnumValN(Logger::LogType::None, "none", "No logging"),
      clEnumValN(Logger::LogType::Printf, "printf", "printf()-based logging"),
      clEnumValN(Logger::LogType::Libxo, "libxo", "libxo-based logging"),
    clEnumValEnd),
    cl::init(Logger::LogType::None));

  struct OptPass : public ModulePass {
    static char ID;
    OptPass() : ModulePass(ID), PolFile(PolicyFile::Open()) {}

    bool runOnModule(Module&) override;

    llvm::ErrorOr<std::unique_ptr<PolicyFile>> PolFile;
  };
}


bool OptPass::runOnModule(Module &Mod)
{
  if (std::error_code err = PolFile.getError()) {
    errs() << "Error opening LOOM policy file: " << err.message() << "\n";
    return false;
  }

  assert(*PolFile);
  Policy& P = **PolFile;

  Instrumenter::NameFn Name =
    [&P](const std::vector<std::string>& Components)
    {
      return P.InstrName(Components);
    };
  std::unique_ptr<Instrumenter> Instr(Instrumenter::Create(Mod, Name, LogType));

  bool ModifiedIR = false;

  //
  // In order to keep from invalidating iterators or instrumenting our
  // instrumentation, we need to decide on the instruction-oriented
  // instrumentation points like calls before we actually instrument them.
  //
  std::unordered_map<CallInst*, std::vector<Policy::Direction>> Calls;

  for (auto& Fn : Mod) {
    // Do we need to instrument this function?
    vector<Policy::Direction> Directions = P.FunctionInstrumentation(Fn);
    if (not Directions.empty()) {
      Instr->Instrument(Fn, Directions);
    }

    for (auto& Inst : instructions(Fn)) {
      // Is this a call to instrument?
      if (CallInst* Call = dyn_cast<CallInst>(&Inst)) {
        Function *Target = Call->getCalledFunction();
        if (not Target)
          continue; // TODO: support indirect targets

        vector<Policy::Direction> Directions = P.CallInstrumentation(*Target);
        if (not Directions.empty())
          Calls.emplace(Call, Directions);
      }
    }
  }

  //
  // Instrument function calls:
  //
  for (auto& i : Calls) {
    ModifiedIR |= Instr->Instrument(i.first, i.second);
  }

  return ModifiedIR;
}

char OptPass::ID = 0;
static RegisterPass<OptPass> X("loom", "Loom instrumentation", false, false);
