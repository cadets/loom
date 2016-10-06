; \file  missing-phi.ll
; \brief Regression test for error that manifested as
;        "PHINode should have one entry for each predecessor"
;
; This test was derived from bsdgrep.full.bc using LLVM's bugpoint.
;
; TODO: set block_structure to true
;
; Commands for llvm-lit:
; RUN: %loom -O2 -S %s -loom-file %s.policy -o %t.instr.ll

; ModuleID = 'bugpoint-reduced-simplified.bc'
source_filename = "bugpoint-output-2c605a7.bc"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-freebsd11.0"

define void @tre_match_fast() {
entry:
  %shift = alloca i32, align 4
  %j = alloca i64, align 8
  br label %do.body

do.body:
  br i1 undef, label %if.then220, label %if.end405

if.then220:
  store i32 1, i32* %shift, align 4
  br label %do.cond

if.end405:
  %tobool430 = trunc i8 undef to i1
  br i1 %tobool430, label %if.end462, label %if.end405

if.end462:
  %0 = load i32, i32* %shift, align 4
  store i32 %0, i32* %shift, align 4
  %sub584 = sub nsw i64 undef, undef
  store i64 %sub584, i64* %j, align 8
  br label %do.cond

do.cond:
  %1 = load i64, i64* %j, align 8
  br label %do.body
}

; Function Attrs: noreturn nounwind
declare void @llvm.trap() #0

attributes #0 = { noreturn nounwind }
