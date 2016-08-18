//! @file PolicyFile.hh  Declaration of @ref loom::PolicyFile.
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

#ifndef LOOM_POLICY_FILE_H
#define LOOM_POLICY_FILE_H

#include "Policy.hh"

#include <llvm/Support/ErrorOr.h>


namespace loom {


class PolicyFile : public Policy
{
  public:
  /// A structure that contains policy data parsed from a file.
  struct PolicyFileData;

  virtual ~PolicyFile();

  /// Open a policy file.
  static llvm::ErrorOr<std::unique_ptr<PolicyFile>> Open(std::string Filename);

  InstrStrategy::Kind Strategy() const override;

  SimpleLogger::LogType Logging() const override;

  Policy::Directions
    CallHooks(const llvm::Function&) const override;

  Policy::Directions
    FnHooks(const llvm::Function&) const override;

  bool FieldReadHook(const llvm::StructType&, unsigned int) const override;

  bool FieldWriteHook(const llvm::StructType&, unsigned int) const override;

  std::string InstrName(const std::vector<std::string>&) const override;

  private:
  PolicyFile(const PolicyFileData&);

  std::unique_ptr<PolicyFileData> Policy;
};

} // namespace loom

#endif /* LOOM_POLICY_FILE_H */
