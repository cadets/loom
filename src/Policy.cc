//! @file Policy.cc  Definition of @ref loom::Policy.
/*
 * Copyright (c) 2016-2017 Jonathan Anderson
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

#include "KTraceLogger.hh"
#include "Policy.hh"
using namespace llvm;
using namespace loom;
using std::unique_ptr;


Policy::~Policy()
{
}


std::vector<unique_ptr<Logger>> Policy::Loggers(Module& Mod) const
{
  std::vector<unique_ptr<Logger>> Loggers;

  auto SimpleLogType = this->Logging();

  if (SimpleLogType != SimpleLogger::LogType::None) {
    Loggers.push_back(SimpleLogger::Create(Mod, SimpleLogType));
  }

  unique_ptr<Serializer> Serial = this->Serialization(Mod);

  switch (this->KTrace()) {
  case Policy::KTraceTarget::Kernel:
    Loggers.emplace_back(new KTraceLogger(Mod, std::move(Serial), true));
    break;

  case Policy::KTraceTarget::Userspace:
    Loggers.emplace_back(new KTraceLogger(Mod, std::move(Serial), false));
    break;

  case Policy::KTraceTarget::None:
    break;
  }

  return Loggers;
}
