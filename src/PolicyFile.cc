//! @file PolicyFile.cc  Definition of @ref PolicyFile.
/*
 * Copyright (c) 2015 Jonathan Anderson
 * All rights reserved.
 *
 * This software was developed at Memorial University under the
 * NSERC Discovery program (RGPIN-2015-06048).
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
#include "Strings.hh"

#include <llvm/IR/Function.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/YAMLTraits.h>

using namespace llvm;
using namespace loom;
using std::string;
using std::unique_ptr;
using std::vector;


//
// Data that can be represented in an instrumentation description file:
//

/// A description of how to instrument a function.
struct FnInstrumentation
{
  /// Function name (as named by LLVM, possibly language-mangled).
  string Name;

  /// Instrumentation that should be applied to calls to this function.
  Policy::Directions Call;

  /// Instrumentation that should be applied to the body of this function
  /// (prologue and epilogue / return points).
  Policy::Directions Body;
};

/// Everything contained in an instrumentation description file.
struct PolicyFile::PolicyFileData
{
  /// Prefix to prepend to all instrumentation hooks (e.g., "__loom").
  string HookPrefix;

  /// Function instrumentation.
  vector<FnInstrumentation> Functions;
};


//
// YAML conversion details:
//

/// Converts a vector to/from YAML.
template<class T>
struct yaml::SequenceTraits<vector<T>> {
  static size_t size(yaml::IO&, vector<T>& V) { return V.size(); }
  static T& element(IO&, vector<T>& V, size_t I) {
    if (I >= V.size())
      V.resize(I + 1);

    return V[I];
  }
};

/// Converts a Policy::Direction to/from YAML.
template <>
struct yaml::ScalarEnumerationTraits<Policy::Direction> {
  static void enumeration(yaml::IO &io, Policy::Direction& Dir) {
    io.enumCase(Dir, "entry",  Policy::Direction::In);
    io.enumCase(Dir, "exit", Policy::Direction::Out);
  }
};

/// Converts FnInstrumentation to/from YAML.
template <>
struct yaml::MappingTraits<FnInstrumentation> {
  static void mapping(yaml::IO &io, FnInstrumentation &fn) {
    io.mapOptional("name",        fn.Name);
    io.mapOptional("caller",      fn.Call);
    io.mapOptional("callee",      fn.Body);
  }
};

/// Converts PolicyFileData to/from YAML.
template <>
struct yaml::MappingTraits<PolicyFile::PolicyFileData> {
  static void mapping(yaml::IO &io, PolicyFile::PolicyFileData &policy) {
    io.mapOptional("hook_prefix", policy.HookPrefix, string("__loom"));
    io.mapOptional("functions",   policy.Functions);
  }
};


//
// PolicyFile implementation:
//

namespace loom {

PolicyFile::PolicyFile(const PolicyFileData& P)
  : Policy(unique_ptr<PolicyFileData> { new PolicyFileData(P) })
{
}

PolicyFile::~PolicyFile()
{
}


ErrorOr<unique_ptr<PolicyFile>> PolicyFile::Open(string Filename)
{
  auto F = MemoryBuffer::getFile(Filename);
  if (std::error_code ec = F.getError())
    return ec;

  PolicyFileData policy;
  yaml::Input In(*F.get());
  In >> policy;

  if (std::error_code ec = In.error()) {
    return ec;
  }

  return unique_ptr<PolicyFile> { new PolicyFile(policy) };
}


Policy::Directions
PolicyFile::CallInstrumentation(const llvm::Function& Fn) const
{
  StringRef Name = Fn.getName();

  for (FnInstrumentation& F : Policy->Functions)
  {
    if (F.Name == Name)
    {
      return F.Call;
    }
  }

  return Policy::Directions();
}


Policy::Directions
PolicyFile::FunctionInstrumentation(const llvm::Function& Fn) const
{
  StringRef Name = Fn.getName();

  for (FnInstrumentation& F : Policy->Functions)
  {
    if (F.Name == Name)
    {
      return F.Body;
    }
  }

  return Policy::Directions();
}


string PolicyFile::InstrName(const vector<string>& Components) const
{
  vector<string> FullName(1, Policy->HookPrefix);
  FullName.insert(FullName.end(), Components.begin(), Components.end());

  return Join(FullName, "_");
}

} // namespace loom
