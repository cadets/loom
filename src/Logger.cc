//! @file Logger.cc  Definition of @ref loom::Logger.
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

#include "Logger.hh"

#include <llvm/IR/Module.h>
#include <llvm/IR/TypeBuilder.h>

#include <sstream>

using namespace llvm;
using namespace loom;
using std::string;
using std::unique_ptr;
using std::vector;


namespace {
  //! A logger that calls libxo's `xo_emit()`.
  class LibxoLogger : public SimpleLogger {
  public:
    LibxoLogger(Module& Mod) : SimpleLogger(Mod) {}

    StringRef FunctionName() const override { return "xo_emit"; }
    Value* CreateFormatString(IRBuilder<>&, StringRef Prefix,
                              ArrayRef<Value*> Params,
                              StringRef Suffix, bool SuppressUniq) override;
  };

  //! A logger that calls `printf()`.
  class PrintfLogger : public SimpleLogger {
  public:
    PrintfLogger(Module& Mod) : SimpleLogger(Mod) {}

    StringRef FunctionName() const override { return "printf"; }
    Value* CreateFormatString(IRBuilder<>&, StringRef Prefix,
                              ArrayRef<Value*> Params,
                              StringRef Suffix, bool SuppressUniq) override;
  };

  //! A logger that calls my `stderr_printf()`.
  class Stderr_PrintfLogger : public SimpleLogger {
  public:
    Stderr_PrintfLogger(Module& Mod) : SimpleLogger(Mod) {}

    StringRef FunctionName() const override { return "stderr_printf"; }
    Value* CreateFormatString(IRBuilder<>&, StringRef Prefix,
                              ArrayRef<Value*> Params,
                              StringRef Suffix, bool SuppressUniq) override;
  };

} // anonymous namespace


Logger::~Logger()
{
}


unique_ptr<SimpleLogger> SimpleLogger::Create(Module& Mod, LogType Log) {
  switch (Log) {
  case LogType::Printf:
    return unique_ptr<SimpleLogger>(new PrintfLogger(Mod));

  case LogType::Stderr_Printf:
    return unique_ptr<SimpleLogger>(new Stderr_PrintfLogger(Mod));

  case LogType::Libxo:
    return unique_ptr<SimpleLogger>(new LibxoLogger(Mod));

  case LogType::None:
    return unique_ptr<SimpleLogger>();
  }
}


Value* SimpleLogger::Log(Instruction *I, ArrayRef<Value*> Values,
                         StringRef /*Name*/, StringRef Description,
                         bool SuppressUniqueness) {

  // Call the printf-like logging function, ignoring the machine-readable name.
  IRBuilder<> B(I);
  return Call(B, Description, Values, "\n", SuppressUniqueness);
}


CallInst* SimpleLogger::Call(IRBuilder<>& Builder, StringRef Prefix,
                             ArrayRef<Value*> Values, StringRef Suffix,
                             bool SuppressUniqueness) {

  vector<Value*> Args = Adapt(Values, Builder);

  Value *FormatString = CreateFormatString(Builder, Prefix, Args, Suffix,
                                           SuppressUniqueness);
  Args.emplace(Args.begin(), FormatString);

  return Builder.CreateCall(GetFunction(), Args);
}


Function* SimpleLogger::GetFunction() {
  const string Name = FunctionName();

  Function *Fn = Mod.getFunction(Name);
  if (Fn)
    return Fn;

  return Function::Create(GetType(), Function::ExternalLinkage, Name, &Mod);
}


FunctionType* SimpleLogger::GetType() {
  return TypeBuilder<int(const char *, ...), false>::get(Mod.getContext());
}


vector<Value*> SimpleLogger::Adapt(ArrayRef<Value*> Values, IRBuilder<>& B) {
  vector<Value*> Adapted;

  for (Value *V : Values) {
    if (V->getType()->isFloatTy()) {
      V = B.CreateFPExt(V, B.getDoubleTy());
    }

    Adapted.push_back(V);
  }

  return Adapted;
}


Value*
LibxoLogger::CreateFormatString(IRBuilder<>& Builder, StringRef Prefix,
                                ArrayRef<Value*> Values, StringRef Suffix,
                                bool SuppressUniqueness) {

  std::stringstream FormatString;

  FormatString << Prefix.str();

  for (Value *V : Values) {
    const string Name = V->getName();
    Type *T = V->getType();

    // xo can humanize values (e.g., 41025981 -> 41M), but we don't want to
    // do this with pointer values (e.g., 0x7fff01... -> 128T) or
    // floating-point numbers (13.415235 -> 13).
    const bool Humanize = T->isIntegerTy();

    FormatString
      << "{P: }"                 // padding
      << "{"
      << (Humanize ? "h" : "")
      << ":"
      << (SuppressUniqueness ? "" : Name)
      << "/"
      ;

    FormatString << "%";

    if (T->isIntegerTy(8)) {
      FormatString << "c";

    } else if (T->isIntegerTy()) {
      const unsigned Bits = dyn_cast<IntegerType>(T)->getBitWidth();

      if (Bits > 64) {
        FormatString << "ll";
      } else if (Bits > 32) {
        FormatString << "l";
      }

      FormatString << "d";

    } else if (T->isFloatTy() || T->isDoubleTy()) {
      FormatString << "f";

    } else if (T->isPointerTy()) {
      FormatString << "p";

    } else {
      assert(false);
    }

    FormatString << "}"; // close the format string
  }

  FormatString << Suffix.str();

  const string Str = FormatString.str();

  auto i = FormatStrings.find(Str);
  if (i != FormatStrings.end()) {
    return i->second;
  }

  Value *Ptr = Builder.CreateGlobalStringPtr(Str);
  auto *GV = dyn_cast<GlobalVariable>(Ptr->stripInBoundsConstantOffsets());
  GV->setUnnamedAddr(GlobalValue::UnnamedAddr::Global);
  GV->setAlignment(1);

  FormatStrings[Str] = Ptr;

  return Ptr;
}


Value*
PrintfLogger::CreateFormatString(IRBuilder<>& Builder, StringRef Prefix,
                                 ArrayRef<Value*> Values, StringRef Suffix,
                                 bool /*SuppressUniqueness*/) {

  std::stringstream FormatString;

  FormatString << Prefix.str();

  for (Value *V : Values) {
    Type *T = V->getType();

    FormatString << " %";

    if (T->isIntegerTy(8)) {
      FormatString << "c";

    } else if (T->isIntegerTy()) {
      const unsigned Bits = dyn_cast<IntegerType>(T)->getBitWidth();

      if (Bits > 64) {
        FormatString << "ll";
      } else if (Bits > 32) {
        FormatString << "l";
      }

      FormatString << "d";

    } else if (T->isFloatTy() || T->isDoubleTy()) {
      FormatString << "f";

    } else if (T->isPointerTy()) {
      FormatString << "p";

    } else {
      assert(false);
    }
  }

  FormatString << Suffix.str();

  return Builder.CreateGlobalStringPtr(FormatString.str());
}

Value*
Stderr_PrintfLogger::CreateFormatString(IRBuilder<>& Builder, StringRef Prefix,
                                 ArrayRef<Value*> Values, StringRef Suffix,
                                 bool /*SuppressUniqueness*/) {

  std::stringstream FormatString;

  FormatString << Prefix.str();

  for (Value *V : Values) {
    Type *T = V->getType();

    FormatString << " %";

    if (T->isIntegerTy(8)) {
      FormatString << "c";

    } else if (T->isIntegerTy()) {
      const unsigned Bits = dyn_cast<IntegerType>(T)->getBitWidth();

      if (Bits > 64) {
        FormatString << "ll";
      } else if (Bits > 32) {
        FormatString << "l";
      }

      FormatString << "d";

    } else if (T->isFloatTy() || T->isDoubleTy()) {
      FormatString << "f";

    } else if (T->isPointerTy()) {
      FormatString << "p";

    } else {
      assert(false);
    }
  }

  FormatString << Suffix.str();

  return Builder.CreateGlobalStringPtr(FormatString.str());
}
