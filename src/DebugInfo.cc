//! @file DebugInfo.cc  Definition of DebugInfo class.
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

#include "DebugInfo.hh"

#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;
using namespace loom;


DebugInfo::DebugInfo(llvm::Module& M)
  : Mod(M), DbgDeclare(Mod.getFunction("llvm.dbg.declare"))
{
  Mod.getMDKindNames(MetadataKinds);

  if (DbgDeclare) {
    // Examine all calls to @llvm.dbg.declare() to find metadata:
    for (auto& Use : DbgDeclare->uses()) {
      auto *Dbg = dyn_cast<DbgDeclareInst>(Use.getUser());
      assert(Dbg && "call to llvm.dbg.declare must be a DbgDeclareInst");
      DbgDecls[Dbg->getAddress()].push_back(Dbg->getVariable());
    }
  }
}


bool DebugInfo::ModuleHasFullDebugInfo() const
{
  if (not DbgDeclare)
    return false;

  auto *ModuleMetadata = Mod.getNamedMetadata("llvm.dbg.cu");
  if (not ModuleMetadata)
    return false;

  if (auto *DI = Get<DICompileUnit>(ModuleMetadata)) {
    using DICompileUnit::DebugEmissionKind::FullDebug;

    return DI->getEmissionKind() == FullDebug and not DI->isOptimized();
  }

  return false;
}


std::string DebugInfo::FieldName(GetElementPtrInst *GEP)
{
  // Trace back to a variable with debug metadata.
  SmallVector<size_t, 4> GEPOffsets;
  const DIVariable *Var = Trace(GEP, GEPOffsets);
  if (Var == nullptr) {
    errs() << "WARNING: unable to trace GEP ";
    GEP->print(errs(), true);
    errs() << " back to a variable\n";

    return "";
  }

  const DIType *T = dyn_cast<DIType>(Var->getType());
  assert(T && "DIVariable::getType() should return a DIType");

  do {
    switch (T->getTag()) {
    case dwarf::DW_TAG_array_type: {
      auto *CT = dyn_cast<DICompositeType>(T);
      T = dyn_cast<DIType>(CT->getBaseType());
      break;
    }

    case dwarf::DW_TAG_pointer_type: {
      auto *DT = dyn_cast<DIDerivedType>(T);
      T = dyn_cast<DIType>(DT->getBaseType());
      break;
    }

    case dwarf::DW_TAG_structure_type: {
      assert(not GEPOffsets.empty());

      size_t Index = GEPOffsets.back();
      GEPOffsets.pop_back();

      auto *CT = dyn_cast<DICompositeType>(T);
      auto *Member = dyn_cast<DIDerivedType>(CT->getElements()[Index]);

      if (GEPOffsets.empty()) {
        return Member->getName();
      } else {
        T = dyn_cast<DIType>(Member->getBaseType());
      }

      break;
    }

    default:
      errs() << "ERROR: unhandled debug info tag: " << T->getTag() << "\n";
      return "";
    }

  } while (T);

  return "";
}


const DIVariable*
DebugInfo::Trace(GetElementPtrInst *GEP, SmallVectorImpl<size_t> &Offsets)
{
  // Walk backwards from GEP to source until we find a variable with
  // debug metadata (or die trying, having reached the end of the GEP chain).
  do
  {
    // If GEP'ing the address of a structure field, track which field
    // index we are working with. 
    if (isa<StructType>(GEP->getSourceElementType())
        and GEP->getNumIndices() == 2) {

      Value *Index = GEP->idx_begin()[1];

      auto *ConstIdx = dyn_cast<ConstantInt>(Index);
      assert(ConstIdx && "struct field indexing must be static");
      Offsets.push_back(ConstIdx->getLimitedValue());
    }

    Value *Ptr = GEP->getPointerOperand()->stripPointerCasts();
    if (auto *Var = Get<DIVariable>(Ptr)) {
      return Var;
    }

    if (auto *Load = dyn_cast<LoadInst>(Ptr)) {
      while (Load != nullptr) {
        Ptr = Load->getPointerOperand();
        Load = dyn_cast<LoadInst>(Ptr);
      }
    }

    GEP = dyn_cast<GetElementPtrInst>(Ptr);
  }
  while (GEP != nullptr);

  return nullptr;
}

