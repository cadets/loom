; \file  sccp-block-fold.ll
; \brief Regression test for constant propagation of degenerate block branches,
;        leading to "Didn't fold away reference to block!" in
;        llvm::runIPSCCP at lib/Transforms/Scalar/SCCP.cpp:1867.
;
; This test was derived from bsdgrep.full.bc using LLVM's bugpoint.
;
; XFAIL: *
;
; Commands for llvm-lit:
; RUN: %loom -O2 -S %s -loom-file %s.policy -o %t.instr.ll

target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-freebsd11.0"

define void @fastcmp() {
entry:
  br i1 undef, label %a, label %b

a:
  %0 = load i8*, i8** undef, align 8
  br label %b

b:
  %cond = phi i8* [ %0, %a ], [ undef, %entry ]
  ret void
}
