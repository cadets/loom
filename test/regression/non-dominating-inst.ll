; \file  non-dominating-inst.ll
; \brief Regression test for Phi node handling, leading to
;        "Instruction does not dominate all uses" error.
;
; This test was derived from bsdgrep.full.bc using LLVM's bugpoint.
;
; Commands for llvm-lit:
; RUN: %loom -S %s -loom-file %s.policy -o %t.instr.ll

target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-freebsd11.0"

define void @fastcmp() {
entry:
  br i1 undef, label %cond.true, label %cond.false

cond.true:                                        ; preds = %entry
  %0 = load i8*, i8** undef, align 8
  br label %cond.end

cond.false:                                       ; preds = %entry
  br label %cond.end

cond.end:                                         ; preds = %cond.false, %cond.true
  %cond = phi i8* [ %0, %cond.true ], [ undef, %cond.false ]
  br label %for.cond

for.cond:                                         ; preds = %for.cond, %cond.end
  br label %for.cond
}
