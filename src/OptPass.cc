//! @file OptPass.cc  Definition of Loom's LLVM @b opt pass.
/*
 * Copyright (c) 2016 Jonathan Anderson
 * Copyright (c) 2016 Cem Kilic
 * Copyright (c) 2018 Brian Kidney
 * All rights reserved.
 *
 * This software was developed by BAE Systems, the University of Cambridge
 * Computer Laboratory, and Memorial University under DARPA/AFRL contract
 * FA8650-15-C-7558 ("CADETS"), as part of the DARPA Transparent Computing
 * (TC) research program.
 *
 * Copyright (c) 2018 Stephen Lee
 * All rights reserved.
 *
 * This software was developed by SRI International, Purdue University,
 * University of Wisconsin and University of Georgia  under DARPA/AFRL
 * contract FA8650-15-C-7562 ("TRACE"), as part of the DARPA Transparent
 * Computing (TC) research program.
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

#include "DebugInfo.hh"
#include "IRUtils.hh"
#include "Instrumenter.hh"
#include "PolicyFile.hh"
#include "Metadata.hh"
#include "Transform.hh"

#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"

#include <unordered_map>

using namespace llvm;
using namespace loom;
using std::string;
using std::unique_ptr;
using std::vector;

namespace {
/// Name of the YAML-based instrumentation policy file.
cl::opt<string> PolicyFilename("loom-file",
                               cl::desc("instrumentation policy file"),
                               cl::value_desc("filename"),
                               cl::init("loom.policy"));

struct OptPass : public ModulePass {
  static char ID;
  OptPass() : ModulePass(ID), PolFile(PolicyFile::Open(PolicyFilename)) {}

  bool runOnModule(Module &) override;

  llvm::ErrorOr<unique_ptr<PolicyFile>> PolFile;
};
} // namespace

bool OptPass::runOnModule(Module &Mod) {
  if (std::error_code err = PolFile.getError()) {
    errs() << "Error opening LOOM policy file '" << PolicyFilename
           << "': " << err.message() << "\n";
    return false;
  }

  DebugInfo Debug(Mod);
  if (not Debug.ModuleHasFullDebugInfo()) {
    errs() << "Warning: module missing metadata, instrumentation may be "
              "incomplete\n";
  }

  assert(*PolFile);
  Policy &P = **PolFile;

  Instrumenter::NameFn Name = [&P](const std::vector<std::string> &Components) {
    return P.InstrName(Components);
  };

  auto S = InstrStrategy::Create(P.Strategy(), P.UseBlockStructure());

  for (auto &L : P.Loggers(Mod)) {
    S->AddLogger(std::move(L));
  }

  unique_ptr<Instrumenter> Instr(Instrumenter::Create(Mod, Name, std::move(S)));

  //
  // In order to keep from invalidating iterators or instrumenting our
  // instrumentation, we need to decide on the instruction-oriented
  // instrumentation points like calls before we actually instrument them.
  //
  std::vector<Instruction *> AllInstructions;

  std::unordered_map<Function *, Policy::Directions> Functions;
  std::unordered_map<Function *, loom::Metadata> FnMetadata;
  std::unordered_map<Function *, vector<loom::Transform>> FnTransforms;
  std::unordered_map<CallInst *, Policy::Directions> Calls;
  std::unordered_map<CallInst *, std::string> CallReplacements;

  typedef std::pair<GetElementPtrInst *, std::string> NamedGEP;
  std::unordered_map<LoadInst *, NamedGEP> FieldReads;
  std::unordered_map<StoreInst *, NamedGEP> FieldWrites;

  std::unordered_map<LoadInst *, NamedGEP> GlobalReads;
  std::unordered_map<StoreInst *, NamedGEP> GlobalWrites;

  std::unordered_map<Instruction *, const DIVariable *> PointerInsts;

  Function *Main = nullptr;

  for (auto &Fn : Mod) {
    // Store a reference to main for initialization code
    if (Fn.getName() == "main") {
      Main = &Fn;
    }

    // Do we need to instrument this function?
    Policy::Directions Directions = P.FnHooks(Fn);
    if (not Directions.empty()) {
      Functions.emplace(&Fn, Directions);

      auto Md = P.InstrMetadata(Fn);
      if (not Md.Name.empty() && not Md.Id == 0) {
        FnMetadata.emplace(&Fn, Md);
      }
      auto Transforms = P.InstrTransforms(Fn);
      if (not Md.Name.empty() && not Md.Id == 0) {
        FnTransforms.emplace(&Fn, Transforms);
      }
    }

    for (auto &Inst : instructions(Fn)) {
      if (P.InstrumentAll()) {
        AllInstructions.push_back(&Inst);
      }

      if (P.InstrumentPointerInsts()) {
        if (isa<StoreInst>(&Inst) || isa<LoadInst>(&Inst) ||
            isa<GetElementPtrInst>(&Inst)) {

          Value *Ptr = nullptr;
          if (StoreInst *store = dyn_cast<StoreInst>(&Inst)) {
            Ptr = store->getPointerOperand();
          } else if (LoadInst *load = dyn_cast<LoadInst>(&Inst)) {
            Ptr = load->getPointerOperand();
          } else if (GetElementPtrInst *gep =
                         dyn_cast<GetElementPtrInst>(&Inst)) {
            Ptr = gep->getPointerOperand();
          }

          const DIVariable *Var = Debug.Get<DIVariable>(Ptr);
          if (auto *G = llvm::dyn_cast<llvm::GlobalVariable>(Ptr)) {
            Var = Debug.GetGlobalDIVariable(G);
          }

          PointerInsts[&Inst] = Var;
        }

        if (BitCastInst *bc = dyn_cast<BitCastInst>(&Inst)) {
          Type *tau = bc->getType();
          // If dest type is a pointer, the source type must be, too
          if (isa<PointerType>(tau)) {
            PointerInsts[bc] = nullptr;
          }
        }
      }

      if (auto *GEP = dyn_cast<GetElementPtrInst>(&Inst)) {
        if (auto *ST = dyn_cast<StructType>(GEP->getSourceElementType())) {
          // A GEP used for structure field lookup should have indices
          // 0 and i, where i is the field number (not byte index).
          if (GEP->getNumIndices() != 2 or !GEP->hasAllConstantIndices())
            continue;

          if (not P.StructTypeMatters(*ST))
            continue;

          std::string FieldName = Debug.FieldName(GEP);
          assert(not FieldName.empty());

          const bool HookReads = P.FieldReadHook(*ST, FieldName);
          const bool HookWrites = P.FieldWriteHook(*ST, FieldName);

          if (not HookReads and not HookWrites) {
            continue;
          }

          for (auto &Use : GEP->uses()) {
            User *U = Use.getUser();

            if (HookReads) {
              if (auto *Load = dyn_cast<LoadInst>(U)) {
                FieldReads[Load] = {GEP, FieldName};
              }
            }

            if (HookWrites) {
              if (auto *Store = dyn_cast<StoreInst>(U)) {
                FieldWrites[Store] = {GEP, FieldName};
              }
            }
          }
        }
        if (isa<GlobalVariable>(GEP->getPointerOperand())) {
          Value *V = GEP->getPointerOperand();
          if (not P.GlobalValueMatters(*V))
            continue;

          const bool HookReads = P.GlobalReadHook(*V);
          const bool HookWrites = P.GlobalWriteHook(*V);

          std::string GlobalName = V->getName().str();
          assert(not GlobalName.empty());

          if (not HookReads and not HookWrites)
            continue;

          for (auto &Use : GEP->uses()) {
            User *U = Use.getUser();

            if (HookReads) {
              if (auto *Load = dyn_cast<LoadInst>(U)) {
                GlobalReads[Load] = {GEP, GlobalName};
              }
            }

            if (HookWrites) {
              if (auto *Store = dyn_cast<StoreInst>(U)) {
                GlobalWrites[Store] = {GEP, GlobalName};
              }
            }
          }
        }
      }

      // Is this a call to instrument?
      if (CallInst *Call = dyn_cast<CallInst>(&Inst)) {
        Function *Target = Call->getCalledFunction();
        if (not Target)
          continue; // TODO: support indirect targets

        Policy::Directions Directions = P.CallHooks(*Target);
        if (not Directions.empty())
          Calls.emplace(Call, Directions);
      }
       
	  // Is this a call to replace?
      if (CallInst *Call = dyn_cast<CallInst>(&Inst)) {
        Function *Target = Call->getCalledFunction();
        if (not Target)
          continue; // TODO: support indirect targets

        std::string Replacement = P.ReplaceCall(*Target);
        if (not Replacement.empty())
        {
          CallReplacements.emplace(Call, Replacement);
        }
      }
     
	}

  }

  //
  // Now we actually perform the instrumentation:
  //
  bool ModifiedIR = false;

  for (auto *I : AllInstructions) {
    Instr->Instrument(I);
  }

  for (auto &i : PointerInsts) {
    Instruction *I = i.first;
    const DIVariable *Var = i.second;
    Instr->InstrumentPtrInsts(I, Var);
  }

  for (auto &i : Functions) {
	auto Md = FnMetadata[i.first];
	auto Transforms = FnTransforms[i.first];
    ModifiedIR |= Instr->Instrument(*i.first, i.second, Md, Transforms);
  }

  for (auto &i : Calls) {
    ModifiedIR |= Instr->Instrument(i.first, i.second);
  }

  for (auto &i : CallReplacements) {
    ModifiedIR |= Instr->ReplaceCall(i.first, i.second);
  }

  for (auto &i : FieldReads) {
    LoadInst *Load = i.first;
    GetElementPtrInst *GEP = i.second.first;
    StringRef FieldName = i.second.second;

    ModifiedIR |= Instr->Instrument(GEP, Load, FieldName);
  }

  for (auto &i : FieldWrites) {
    StoreInst *Store = i.first;
    GetElementPtrInst *GEP = i.second.first;
    StringRef FieldName = i.second.second;

    ModifiedIR |= Instr->Instrument(GEP, Store, FieldName);
  }

  for (auto &i : GlobalReads) {
    LoadInst *Load = i.first;
    GetElementPtrInst *GEP = i.second.first;
    StringRef Name = i.second.second;

    ModifiedIR |= Instr->Instrument(GEP, Load, Name);
  }

  for (auto &i : GlobalWrites) {
    StoreInst *Store = i.first;
    GetElementPtrInst *GEP = i.second.first;
    StringRef Name = i.second.second;

    ModifiedIR |= Instr->Instrument(GEP, Store, Name);
  }

  if (ModifiedIR) {
    // Add required initialization for loggers to main
    if (Main != nullptr) {
      ModifiedIR |= Instr->InitializeLoggers(*Main);
    }

  }
  return ModifiedIR;
}

char OptPass::ID = 0;
static RegisterPass<OptPass> X("loom", "Loom instrumentation", false, false);
