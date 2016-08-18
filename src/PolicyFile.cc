//! @file PolicyFile.cc  Definition of @ref PolicyFile.
/*
 * Copyright (c) 2015-2016 Jonathan Anderson
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

namespace {

template<class T>
bool vecContains(const std::vector<T>& V, const T& Val) {
  return (std::find(V.begin(), V.end(), Val) != V.end());
}

}

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

/// An operation that can be performed on a structure field
enum class FieldOperation
{
  Read,
  Write,
};

/// A description of how to instrument a function.
struct FieldInstrumentation
{
  /// Numeric ID of the field (as name may not be available).
  unsigned int Id;

  /// Operations (read/write) that should be instrumented.
  vector<FieldOperation> Operations;
};

/// A description of how to instrument fields within a structure.
struct StructInstrumentation
{
  /// Name of the structure containing the field (as named by LLVM, possibly mangled).
  string Name;

  /// Instrumentation that should be applied to calls to this function.
  vector<FieldInstrumentation> Fields;
};


/// Everything contained in an instrumentation description file.
struct PolicyFile::PolicyFileData
{
  /// Prefix to prepend to all instrumentation hooks (e.g., "__loom").
  string HookPrefix;

  /// How to instrument: inline, via callout function, etc.
  InstrStrategy::Kind Strategy;

  /// Simple (non-serializing) logging strategy.
  SimpleLogger::LogType Logging;

  /// KTrace-based logging.
  Policy::KTraceTarget KTrace;

  /// Function instrumentation.
  vector<FnInstrumentation> Functions;

  /// Structure field instrumentation.
  vector<StructInstrumentation> Structures;
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

/// Converts an InstrStrategy::Kind to/from YAML.
template <>
struct yaml::ScalarEnumerationTraits<InstrStrategy::Kind> {
  static void enumeration(yaml::IO &io, InstrStrategy::Kind& K) {
    io.enumCase(K, "callout",  InstrStrategy::Kind::Callout);
    io.enumCase(K, "inline", InstrStrategy::Kind::Inline);
  }
};

/// Converts an KTraceTarget to/from YAML.
template <>
struct yaml::ScalarEnumerationTraits<Policy::KTraceTarget> {
  static void enumeration(yaml::IO &io, Policy::KTraceTarget& T) {
    io.enumCase(T, "kernel",  Policy::KTraceTarget::Kernel);
    io.enumCase(T, "utrace", Policy::KTraceTarget::Userspace);
    io.enumCase(T, "none", Policy::KTraceTarget::None);
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

/// Converts a SimpleLogger::LogType to/from YAML.
template <>
struct yaml::ScalarEnumerationTraits<SimpleLogger::LogType> {
  static void enumeration(yaml::IO &io, SimpleLogger::LogType& T) {
    io.enumCase(T, "printf",  SimpleLogger::LogType::Printf);
    io.enumCase(T, "xo", SimpleLogger::LogType::Libxo);
    io.enumCase(T, "none", SimpleLogger::LogType::None);
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

/// Converts a FieldOperation to/from YAML.
template <>
struct yaml::ScalarEnumerationTraits<FieldOperation> {
  static void enumeration(yaml::IO &io, FieldOperation& Dir) {
    io.enumCase(Dir, "read",  FieldOperation::Read);
    io.enumCase(Dir, "write", FieldOperation::Write);
  }
};

/// Converts FieldInstrumentation to/from YAML.
template <>
struct yaml::MappingTraits<FieldInstrumentation> {
  static void mapping(yaml::IO &io, FieldInstrumentation &f) {
    io.mapRequired("id",          f.Id);
    io.mapRequired("operations",  f.Operations);
  }
};

/// Converts StructInstrumentation to/from YAML.
template <>
struct yaml::MappingTraits<StructInstrumentation> {
  static void mapping(yaml::IO &io, StructInstrumentation &s) {
    io.mapRequired("name",        s.Name);
    io.mapRequired("fields",      s.Fields);
  }
};

/// Converts PolicyFileData to/from YAML.
template <>
struct yaml::MappingTraits<PolicyFile::PolicyFileData> {
  static void mapping(yaml::IO &io, PolicyFile::PolicyFileData &policy) {
    io.mapOptional("strategy", policy.Strategy, InstrStrategy::Kind::Callout);
    io.mapOptional("logging", policy.Logging, SimpleLogger::LogType::None);
    io.mapOptional("ktrace", policy.KTrace, Policy::KTraceTarget::None);
    io.mapOptional("hook_prefix", policy.HookPrefix, string("__loom"));
    io.mapOptional("functions",   policy.Functions);
    io.mapOptional("structures",  policy.Structures);
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


InstrStrategy::Kind PolicyFile::Strategy() const
{
  return Policy->Strategy;
}


SimpleLogger::LogType PolicyFile::Logging() const
{
  return Policy->Logging;
}


Policy::KTraceTarget PolicyFile::KTrace() const
{
  return Policy->KTrace;
}


Policy::Directions
PolicyFile::CallHooks(const llvm::Function& Fn) const
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
PolicyFile::FnHooks(const llvm::Function& Fn) const
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

bool
PolicyFile::FieldReadHook(const llvm::StructType& T, unsigned int Id) const
{
  if (not T.getName().startswith("struct.")) {
    assert(T.getName().startswith("union."));
    return false;
  }

  StringRef Name = T.getName().substr(7);

  for (StructInstrumentation& S : Policy->Structures) {
    if (S.Name != Name) {
      continue;
    }

    for (auto& Field : S.Fields) {
      if (Field.Id != Id) {
        return vecContains(Field.Operations, FieldOperation::Read);
      }
    }
  }

  return false;
}


bool
PolicyFile::FieldWriteHook(const llvm::StructType& T, unsigned int Id) const
{
  if (not T.getName().startswith("struct.")) {
    assert(T.getName().startswith("union."));
    return false;
  }

  StringRef Name = T.getName().substr(7);

  for (StructInstrumentation& S : Policy->Structures) {
    if (S.Name != Name) {
      continue;
    }

    for (auto& Field : S.Fields) {
      if (Field.Id == Id) {
        return vecContains(Field.Operations, FieldOperation::Write);
      }
    }
  }

  return false;
}


string PolicyFile::InstrName(const vector<string>& Components) const
{
  vector<string> FullName(1, Policy->HookPrefix);
  FullName.insert(FullName.end(), Components.begin(), Components.end());

  return Join(FullName, "_");
}

} // namespace loom
