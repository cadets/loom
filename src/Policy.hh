//! @file Policy.hh  Definition of @ref loom::Policy.
/*
 * Copyright (c) 2015-2017 Jonathan Anderson
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

#ifndef LOOM_POLICY_H
#define LOOM_POLICY_H

#include "InstrStrategy.hh"
#include "Serializer.hh"

#include <string>
#include <vector>


namespace llvm
{
  class Function;
  class StructType;
}


namespace loom {

/**
 * An instrumentation policy tells us what things to instrument and in
 * which direction (e.g., call vs return).
 */
class Policy
{
  public:
  virtual ~Policy();

  /**
   * The instrumentation strategy that should be employed by this pass.
   *
   * This is currently a pass-level decision, but in the future we may want to
   * employ different strategies for different types of instrumentation within
   * a single module.
   */
  virtual InstrStrategy::Kind Strategy() const = 0;

  //! Create all loggers required by the policy.
  virtual std::vector<std::unique_ptr<Logger>> Loggers(llvm::Module&) const;

  //! Simple (non-serializing) logging.
  virtual SimpleLogger::LogType Logging() const = 0;

  //! Ways that we can use KTrace (or not).
  enum class KTraceTarget { Kernel, Userspace, None };

  //! Should we use ktrace logging?
  virtual KTraceTarget KTrace() const = 0;

  //! How should we serialize data?
  virtual std::unique_ptr<Serializer> Serialization(llvm::Module&) const = 0;

  /**
   * Create instrumentation within BasicBlocks, making the structure of the
   * instrumentation clearer but more complex.
   */
  virtual bool UseBlockStructure() const = 0;

  //! Special case: instrument every single function.
  virtual bool InstrumentAll() const = 0;

  //! A direction that we can instrument: on the way in or on the way out.
  enum class Direction { In, Out };

  //! A set of directions that we might like to instrument something on.
  typedef std::vector<Direction> Directions;

  //! In which directions should calls to a function be instrumented?
  virtual Directions CallHooks(const llvm::Function&) const = 0;

  //! In which directions (preamble/return) should a function be instrumented?
  virtual Directions FnHooks(const llvm::Function&) const = 0;

  /**
   * A structure type is relevant in some way to instrumentation.
   *
   * Since the process of iterating over debug metadata can be expensive,
   * this method allows a policy to express which structure types are to be
   * instrumented in any way.
   */
  virtual bool StructTypeMatters(const llvm::StructType&) const = 0;

  /**
   * Should a read from a structure field be instrumented?
   *
   * @param   T     the type of the structure being read from
   * @param   Field the field being read from within the structure
   */
  virtual bool
    FieldReadHook(const llvm::StructType& T, llvm::StringRef Field) const = 0;

  /**
   * Should a write to a structure field be instrumented?
   *
   * @param   T     the type of the structure being written to
   * @param   Field the field being written to within the structure
   */
  virtual bool
    FieldWriteHook(const llvm::StructType& T, llvm::StringRef Field) const = 0;

  //! Name an instrumentation function for a particular event type.
  virtual std::string
    InstrName(const std::vector<std::string>& Components) const = 0;
};

} // namespace loom

#endif /* LOOM_POLICY_H */
