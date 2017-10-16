//! @file InstrStrategy.hh  Declaration of @ref loom::InstrStrategy.
/*
 * Copyright (c) 2016-2017 Jonathan Anderson
 * All rights reserved.
 *
 * This software was developed by BAE Systems, the University of Cambridge
 * Computer Laboratory, and Memorial University under DARPA/AFRL contract
 * FA8650-15-C-7558 ("CADETS"), as part of the DARPA Transparent Computing
 * (TC) research program, as well as at Memorial University under the
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

#ifndef  LOOM_INSTR_STRATEGY_H
#define  LOOM_INSTR_STRATEGY_H

#include <memory>

#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/StringRef.h>

#include "IRUtils.hh"
#include "Logger.hh"


namespace llvm {
  class Instruction;
}


namespace loom {

class Instrumentation;

//! An instrumentation strategy (inline, callout, etc.).
class InstrStrategy {
public:
  enum class Kind {
    Callout,    //!< Call out to a user-defined instrumentation function.
    Inline,     //!< Add instrumentation inline with the instrumented code.
  };

  virtual ~InstrStrategy();

  /**
   * Create a new instrumentation strategy (callout, inline, etc.).
   *
   * @param   K         Which instrumentation approach to take.
   * @param   UseBlocks Use BasicBlock-based internal structure for
   *                    instrumentation in order to expose the control flow
   *                    among [potentially] different instrumentation actions
   *                    very explicit. This is the old behaviour from TESLA.
   *                    The alternative (if UseBlocks is false) is to generate
   *                    a stream of instructions.
   */
  static std::unique_ptr<InstrStrategy> Create(Kind K, bool UseBlocks);

  //! Add another @ref Logger to the instrumentation we generate.
  void AddLogger(std::unique_ptr<Logger>);

  /**
   * Instrument a particular instruction, returning an @ref Instrumentation
   * object that can be used to create actions.
   *
   * @param  I            The instruction to instrument.
   * @param  Name         The name of the instrumentation to create/find.
   *                      This name will be incorporated in the name of the
   *                      instrumentation function or inline BasicBlock(s),
   *                      so it should be unique to the desired instrumentation
   *                      (e.g., `"__loom_returnfrom_foo"`).
   * @param  Description  A human-readable short description of the
   *                      instrumentation for logging, etc., e.g., `"call foo"`.
   * @param  Params       Static description (names and types) of the values
   *                      that will be passed to the instrumentation.
   * @param  Values       Dynamic values to pass to the instrumentation.
   *                      Must agree in number and type with @b Params and
   *                      @b VarArgs, but may be named differently (e.g., when
   *                      instrumenting function calls, the parameters and
   *                      arguments may have different names).
   * @param  VarArgs      The instrumentation should accept varargs after the
   *                      explicitly-defined @b Params.
   * @param  AfterInst    If true, apply the instrumentation *after* the
   *                      instruction rather than, as by default, before it.
   * @param  SuppressUniqueness  Suppress, to the extent possible, uniqueness
   *                      among constants (e.g., format strings) generated in
   *                      support of instrumentation instances. For example,
   *                      libxo format strings can be generated with names for
   *                      each argument: this flag suppresses such detail.
   *                      This may be necessary when generating great quantities
   *                      of instrumentation (e.g., using `everything`).
   *
   * Example of simple Function Boundary Tracing (where the @b Params and
   * @b Values come from the same place, the target function's parameters):
   *
   * ```cpp
   * Function *Target = // the function you want to instrument
   * vector<Parameter> Params = // get from Target->getArgumentList()
   * vector<Value*> Values = // get from Target->getArgumentList()
   * Instruction *Start = &Target->getBasicBlockList().front().front();
   *
   * Strategy->Instrument(Start, "__foo_hook_fn_" + Target->getName(),
   *                      "calling " + Target->getName(), Params, Values,
   *                      Target->isVarArg());
   * ```
   */
  virtual Instrumentation Instrument(llvm::Instruction *I,
                                     llvm::StringRef Name,
                                     llvm::StringRef Description,
                                     llvm::ArrayRef<Parameter> Params,
                                     llvm::ArrayRef<llvm::Value*> Values,
                                     bool VarArgs = false,
                                     bool AfterInst = false,
                                     bool SuppressUniqueness = false) = 0;

protected:
  InstrStrategy(bool UseBlocks) : UseBlockStructure(UseBlocks) {}

  /**
   * Add code to instrumentation preamble that will log the instrumented values
   * via all of our Logger objects.
   *
   * @returns   the last instruction of the added logging code (or I if none)
   */
  llvm::Value* AddLogging(llvm::Instruction *I,
                          llvm::ArrayRef<llvm::Value*>,
                          llvm::StringRef Name,
                          llvm::StringRef Description,
                          bool SuppressUniqueness);

  /**
   * Use an explicit structure of premable/instrumentation/end BasicBlocks
   * when creating instrumentation.
   */
  const bool UseBlockStructure;

private:
  std::vector<std::unique_ptr<Logger>> Loggers;
};

} // namespace loom

#endif
