; ModuleID = '/usr/home/ck8418/dev/loom/build/test/Output/call-instrumentation.c.tmp.ll'
source_filename = "/usr/home/ck8418/dev/loom/build/test/Output/call-instrumentation.c.tmp.ll"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-freebsd10.1"

@0 = private unnamed_addr constant [13 x i8] c"calling foo(\00"
@1 = private unnamed_addr constant [13 x i8] c"calling foo(\00"
@2 = private unnamed_addr constant [13 x i8] c"calling bar(\00"

; Function Attrs: nounwind uwtable
define void @foo() #0 !dbg !4 {
entry:
  ret void, !dbg !20
}

; Function Attrs: nounwind uwtable
define void @bar() #0 !dbg !8 {
entry:
  ret void, !dbg !21
}

; Function Attrs: nounwind uwtable
define void @baz() #0 !dbg !9 {
entry:
  ret void, !dbg !22
}

; Function Attrs: nounwind uwtable
define i32 @main(i32 %argc, i8** %argv) #0 !dbg !10 {
entry:
  %retval = alloca i32, align 4
  %argc.addr = alloca i32, align 4
  %argv.addr = alloca i8**, align 8
  store i32 0, i32* %retval, align 4
  store i32 %argc, i32* %argc.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %argc.addr, metadata !23, metadata !24), !dbg !25
  store i8** %argv, i8*** %argv.addr, align 8
  call void @llvm.dbg.declare(metadata i8*** %argv.addr, metadata !26, metadata !24), !dbg !27
  call void @__test_hook_call_foo_() #0, !dbg !28
  call void @__test_hook_return_foo_() #0, !dbg !28
  call void @foo(), !dbg !28
  call void @__test_hook_call_bar_() #0, !dbg !29
  call void @bar(), !dbg !29
  call void @baz(), !dbg !30
  ret i32 0, !dbg !31
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

define void @__test_hook_call_foo_() {
entry:
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @0, i32 0, i32 0))
  ret void
}

declare i32 @printf(i8*, ...)

define void @__test_hook_return_foo_() {
entry:
  %0 = call i32 (i8*, ...) @printf.1(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @1, i32 0, i32 0))
  ret void
}

declare i32 @printf.1(i8*, ...)

define void @__test_hook_call_bar_() {
entry:
  %0 = call i32 (i8*, ...) @printf.2(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @2, i32 0, i32 0))
  ret void
}

declare i32 @printf.2(i8*, ...)

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!17, !18}
!llvm.ident = !{!19}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 3.9.0 ", isOptimized: false, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !3)
!1 = !DIFile(filename: "/usr/home/ck8418/dev/loom/build/test/Output/call-instrumentation.c.tmp.c", directory: "/usr/home/ck8418/dev/loom/build/test")
!2 = !{}
!3 = !{!4, !8, !9, !10}
!4 = distinct !DISubprogram(name: "foo", scope: !5, file: !5, line: 31, type: !6, isLocal: false, isDefinition: true, scopeLine: 31, flags: DIFlagPrototyped, isOptimized: false, variables: !2)
!5 = !DIFile(filename: "/usr/home/ck8418/dev/loom/test/call-instrumentation.c", directory: "/usr/home/ck8418/dev/loom/build/test")
!6 = !DISubroutineType(types: !7)
!7 = !{null}
!8 = distinct !DISubprogram(name: "bar", scope: !5, file: !5, line: 34, type: !6, isLocal: false, isDefinition: true, scopeLine: 34, flags: DIFlagPrototyped, isOptimized: false, variables: !2)
!9 = distinct !DISubprogram(name: "baz", scope: !5, file: !5, line: 37, type: !6, isLocal: false, isDefinition: true, scopeLine: 37, flags: DIFlagPrototyped, isOptimized: false, variables: !2)
!10 = distinct !DISubprogram(name: "main", scope: !5, file: !5, line: 40, type: !11, isLocal: false, isDefinition: true, scopeLine: 41, flags: DIFlagPrototyped, isOptimized: false, variables: !2)
!11 = !DISubroutineType(types: !12)
!12 = !{!13, !13, !14}
!13 = !DIBasicType(name: "int", size: 32, align: 32, encoding: DW_ATE_signed)
!14 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !15, size: 64, align: 64)
!15 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !16, size: 64, align: 64)
!16 = !DIBasicType(name: "char", size: 8, align: 8, encoding: DW_ATE_signed_char)
!17 = !{i32 2, !"Dwarf Version", i32 2}
!18 = !{i32 2, !"Debug Info Version", i32 3}
!19 = !{!"clang version 3.9.0 "}
!20 = !DILocation(line: 31, column: 17, scope: !4)
!21 = !DILocation(line: 34, column: 17, scope: !8)
!22 = !DILocation(line: 37, column: 17, scope: !9)
!23 = !DILocalVariable(name: "argc", arg: 1, scope: !10, file: !5, line: 40, type: !13)
!24 = !DIExpression()
!25 = !DILocation(line: 40, column: 10, scope: !10)
!26 = !DILocalVariable(name: "argv", arg: 2, scope: !10, file: !5, line: 40, type: !14)
!27 = !DILocation(line: 40, column: 22, scope: !10)
!28 = !DILocation(line: 44, column: 2, scope: !10)
!29 = !DILocation(line: 49, column: 2, scope: !10)
!30 = !DILocation(line: 54, column: 2, scope: !10)
!31 = !DILocation(line: 57, column: 2, scope: !10)
