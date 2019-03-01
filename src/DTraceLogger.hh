//! @file DTraceLogger.hh  Declaration of @ref DTraceLogger.
/*
 * Copyright (c) 2017 Brian Kidney
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

#ifndef DTRACE_LOGGER_H_
#define DTRACE_LOGGER_H_

#include "Logger.hh"

namespace loom {

class Serializer;

/**
 * A logging technique that writes values to the DTrace framework using
 * Userland Statically Defined Traces (USDT). libusdt is used to create probes
 * at runtime.
 */
class DTraceLogger : public loom::Logger {
public:
  DTraceLogger(llvm::Module& Mod);


  virtual llvm::Value* Log(llvm::Instruction *I, llvm::ArrayRef<llvm::Value*> Values,
                         llvm::StringRef Name, llvm::StringRef Descrip,
                         Metadata Metadata, std::vector<Transform> Transforms,
                         bool /* SuppressUniqueness */) override;
private:
	llvm::Value* ConvertValueToPtr(llvm::IRBuilder<>&, llvm::LLVMContext&, llvm::Value*, llvm::Type*);

};

} // namespace loom

#endif // !DTRACE_LOGGER_H_
