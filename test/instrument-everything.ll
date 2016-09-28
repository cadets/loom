; \file  instrument-everything.ll
; \brief Tests generic instrumentation of all instructions
;
; Commands for llvm-lit:
; RUN: %loom -S %s -loom-file %s.policy -o %t.instr.ll
; RUN: %filecheck -input-file %t.instr.ll %s
; RUN: %llc -filetype=obj %t.instr.ll -o %t.instr.o
; RUN: %clang %ldflags %t.instr.o -o %t.instr
; RUN: %t.instr > %t.output
; RUN: %filecheck -input-file %t.output %s -check-prefix CHECK-OUTPUT

; ModuleID = 'tmp.c'
source_filename = "tmp.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-freebsd12.0"

@.str = private unnamed_addr constant [15 x i8] c"Hello, world!\0A\00", align 1
@.str.1 = private unnamed_addr constant [23 x i8] c"The value of x is: %f\0A\00", align 1

; Function Attrs: nounwind uwtable
define i32 @main(i32 %argc, i8** %argv) #0 !dbg !6 {
entry:
  ; CHECK: %retval = alloca i32, align 4
  %retval = alloca i32, align 4
  ; CHECK: xo_emit({{.*}} %retval
  ; CHECK-OUTPUT: [[INSTR:instrumentation:instruction]]:0x{{[0-f]+}} [[ALLOCA:[0-9]+]] [[RETVAL:0x[0-f]+]]

  ; CHECK: %argc.addr = alloca i32, align 4
  %argc.addr = alloca i32, align 4
  ; CHECK: xo_emit({{.*}} %argc.addr
  ; CHECK-OUTPUT: [[INSTR]]:0x{{[0-f]+}} [[ALLOCA]] [[ARGC_ADDR:0x[0-f]+]]

  ; CHECK: %argv.addr = alloca i8**, align 8
  %argv.addr = alloca i8**, align 8
  ; CHECK: xo_emit({{.*}} %argv.addr
  ; CHECK-OUTPUT: [[INSTR]]:0x{{[0-f]+}} [[ALLOCA]] [[ARGV_ADDR:0x[0-f]+]]

  ; CHECK: %x = alloca double, align 8
  %x = alloca double, align 8
  ; CHECK: xo_emit({{.*}} %x
  ; CHECK-OUTPUT: [[INSTR]]:0x{{[0-f]+}} [[ALLOCA]] [[XPTR:0x[0-f]+]]

  ; CHECK: store i32 0, i32* %retval, align 4
  store i32 0, i32* %retval, align 4
  ; CHECK: xo_emit({{.*}}i32* %retval
  ; CHECK-OUTPUT: [[INSTR]]:0x{{[0-f]+}} [[STORE:[0-9]+]] 0 [[RETVAL]]

  ; CHECK: store i32 %argc, i32* %argc.addr, align 4
  store i32 %argc, i32* %argc.addr, align 4
  ; CHECK: xo_emit({{.*}}i32* %argc.addr
  ; CHECK-OUTPUT: [[INSTR]]:0x{{[0-f]+}} [[STORE]] {{[0-9]+}} [[ARGC_ADDR]]

  call void @llvm.dbg.declare(metadata i32* %argc.addr, metadata !13, metadata !14), !dbg !15

  ; CHECK: store i8** %argv, i8*** %argv.addr, align 8
  store i8** %argv, i8*** %argv.addr, align 8
  ; CHECK: xo_emit({{.*}}i8*** %argv.addr
  ; CHECK-OUTPUT: [[INSTR]]:0x{{[0-f]+}} [[STORE]] {{0x[0-f]+}} [[ARGV_ADDR]]

  call void @llvm.dbg.declare(metadata i8*** %argv.addr, metadata !16, metadata !14), !dbg !17

  ; CHECK: %call = call i32 (i8*, ...) @printf
  %call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([15 x i8], [15 x i8]* @.str, i32 0, i32 0)), !dbg !18
  ; CHECK: xo_emit({{.*}} %call
  ; CHECK-OUTPUT: [[INSTR]]:0x{{[0-f]+}} [[CALL:[0-9]+]] [[LEN:14]] "Hello, world!
  ; CHECK-OUTPUT-NEXT: " [[PRINTF:0x[0-f]+]]

  call void @llvm.dbg.declare(metadata double* %x, metadata !19, metadata !14), !dbg !21

  ; CHECK: store double 0x402ABCEF97BFC839, double* %x, align 8, !dbg !21
  store double 0x402ABCEF97BFC839, double* %x, align 8, !dbg !21
  ; CHECK: xo_emit({{.*}}double* %x
  ; CHECK-OUTPUT: [[INSTR]]:0x{{[0-f]+}} [[STORE]] [[X:[0-9.]+]] [[XPTR:0x[0-f]+]]


  ; Note: Unnamed values like this one can be renumbered arbitrarily once we
  ;       introduce instrumentation (which can itself include unnamed values),
  ;       so we can't check for a literal %0. Instead, we need to check for
  ;       consistency between the value name and what we pass to
  ;       instrumentation.

  ; CHECK: %[[ZERO:[0-9]+]] = load double, double* %x, align 8, !dbg !22
  %0 = load double, double* %x, align 8, !dbg !22
  ; CHECK: xo_emit({{.*}} %[[ZERO]]
  ; CHECK-OUTPUT: [[INSTR]]:0x{{[0-f]+}} [[LOAD:[0-9]+]] [[X]] [[XPTR]]

  ; CHECK: %call1 = call i32 (i8*, ...) @printf
  %call1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str.1, i32 0, i32 0), double %0), !dbg !23
  ; CHECK: xo_emit({{.*}} %call1
  ; CHECK-OUTPUT: [[INSTR]]:0x{{[0-f]+}} [[CALL:[0-9]+]] {{[0-9]+}} "The value of x is:
  ; CHECK-OUTPUT-NEXT: " [[X]] [[PRINTF]]

  ; CHECK: xo_emit({{.*}}, i32 [[BRANCH:2]])
  ; CHECK: br label %foo
  br label %foo

foo:
  %call2 = call i32 @xo_finish(), !dbg !24

  ret i32 0, !dbg !25
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @printf(i8*, ...) #2

declare i32 @xo_finish() #2

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }
attributes #2 = { "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!3, !4}
!llvm.ident = !{!5}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 4.0.0 (http://llvm.org/git/clang.git 3f7a1d0a6dedbbff7cdd804e31063c9c33a837ec) (http://llvm.org/git/llvm.git 20014804ba63d479300bb5c136856d11fbc1bdae)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !2)
!1 = !DIFile(filename: "tmp.c", directory: "/usr/home/jon/CADETS/local/loom/Debug")
!2 = !{}
!3 = !{i32 2, !"Dwarf Version", i32 2}
!4 = !{i32 2, !"Debug Info Version", i32 3}
!5 = !{!"clang version 4.0.0 (http://llvm.org/git/clang.git 3f7a1d0a6dedbbff7cdd804e31063c9c33a837ec) (http://llvm.org/git/llvm.git 20014804ba63d479300bb5c136856d11fbc1bdae)"}
!6 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 5, type: !7, isLocal: false, isDefinition: true, scopeLine: 6, flags: DIFlagPrototyped, isOptimized: false, unit: !0, variables: !2)
!7 = !DISubroutineType(types: !8)
!8 = !{!9, !9, !10}
!9 = !DIBasicType(name: "int", size: 32, align: 32, encoding: DW_ATE_signed)
!10 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64, align: 64)
!11 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !12, size: 64, align: 64)
!12 = !DIBasicType(name: "char", size: 8, align: 8, encoding: DW_ATE_signed_char)
!13 = !DILocalVariable(name: "argc", arg: 1, scope: !6, file: !1, line: 5, type: !9)
!14 = !DIExpression()
!15 = !DILocation(line: 5, column: 14, scope: !6)
!16 = !DILocalVariable(name: "argv", arg: 2, scope: !6, file: !1, line: 5, type: !10)
!17 = !DILocation(line: 5, column: 26, scope: !6)
!18 = !DILocation(line: 7, column: 2, scope: !6)
!19 = !DILocalVariable(name: "x", scope: !6, file: !1, line: 9, type: !20)
!20 = !DIBasicType(name: "double", size: 64, align: 64, encoding: DW_ATE_float)
!21 = !DILocation(line: 9, column: 9, scope: !6)
!22 = !DILocation(line: 10, column: 36, scope: !6)
!23 = !DILocation(line: 10, column: 2, scope: !6)
!24 = !DILocation(line: 12, column: 2, scope: !6)
!25 = !DILocation(line: 14, column: 2, scope: !6)
