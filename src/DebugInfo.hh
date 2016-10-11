//! @file DebugInfo.hh  Declaration of DebugInfo class.
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

#ifndef LOOM_DEBUG_INFO_H
#define LOOM_DEBUG_INFO_H

#include <llvm/IR/Instruction.h>
#include <llvm/IR/Metadata.h>
#include <llvm/IR/ValueMap.h>
#include <llvm/Support/raw_ostream.h>


namespace llvm {
class GetElementPtrInst;
}

namespace loom {

/// Class for finding LLVM debug information within a Module.
class DebugInfo
{
public:
  /// A single LLVM metadata node, with an integer metadata "kind".
  typedef std::pair<unsigned, llvm::MDNode*> DebugNode;

  /// A vector of metadata nodes associated with a Value.
  template<unsigned int Size=4>
  using DebugVec = llvm::SmallVector<DebugNode, Size>;

  DebugInfo(llvm::Module&);

  bool ModuleHasFullDebugInfo() const;

  /// Find the name of a field being looked up by a GetElementPtrInst.
  std::string FieldName(llvm::GetElementPtrInst*);

  template<class DebugType = llvm::Metadata>
  const DebugType* Get(llvm::NamedMDNode *Node) const
  {
    for (auto *MD : Node->operands()) {
      if (auto *DebugInfo = llvm::dyn_cast<DebugType>(MD)) {
        return DebugInfo;
      }
    }

    return nullptr;
  }

  template<class DebugType = llvm::Metadata>
  const DebugType* Get(llvm::Value *V) const
  {
    if (auto *I = llvm::dyn_cast<llvm::Instruction>(V)) {
      llvm::SmallVector<DebugNode, 4> Debug;
      I->getAllMetadata(Debug);

      for (auto i : Debug) {
        if (auto *DebugInfo = llvm::dyn_cast<DebugType>(i.second)) {
          return DebugInfo;
        }
      }
    }

    auto i = DbgDecls.find(V);
    if (i != DbgDecls.end()) {
      for (auto *MD : i->second) {
        if (auto *DebugInfo = llvm::dyn_cast<DebugType>(MD)) {
          return DebugInfo;
        }
      }
    }

    return nullptr;
  }

private:
  /**
   * Trace from a GetElementPtrInst representing a structure field lookup
   * back to a variable that has a DIVariable debug metadata node.
   *
   * The result is a DIVariable for the variable with debug information and
   * a vector that of GEP offsets, representing how we got to a variable from
   * the GEP of interest. Together, these can be used to work forwards again
   * from the DIVariable to a field name in the current GEP.
   *
   * @param[in]   GEP        the structure field lookup to trace
   * @param[out]  Offsets    the vector to fill with GEP offsets
   *
   * @returns     debug metadata for the variable we found
   */
  const llvm::DIVariable* Trace(llvm::GetElementPtrInst *GEP,
                                llvm::SmallVectorImpl<size_t> &Offsets);

  llvm::Module& Mod;
  llvm::Function *DbgDeclare;

  /// Kinds of metadata that are used in this module.
  llvm::SmallVector<llvm::StringRef, 4> MetadataKinds;

  /// Declarations of metadata, i.e., metadata from `@llvm.db.declare()` calls.
  llvm::ValueMap<llvm::Value*, llvm::SmallVector<llvm::Metadata*, 4>> DbgDecls;
};

} // namespace loom

#endif // LOOM_DEBUG_INFO_H
