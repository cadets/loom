; ModuleID = '/usr/home/ck8418/dev/loom/build/test/Output/function-instrumentation.c.tmp.c'
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-freebsd10.1"

; Function Attrs: nounwind uwtable
define i32 @main(i32 %argc, i8** %argv) #0 !dbg !4 {
entry:
  %retval = alloca i32, align 4
  %argc.addr = alloca i32, align 4
  %argv.addr = alloca i8**, align 8
  store i32 0, i32* %retval, align 4
  store i32 %argc, i32* %argc.addr, align 4
  call void @llvm.dbg.declare(metadata i32* %argc.addr, metadata !20, metadata !21), !dbg !22
  store i8** %argv, i8*** %argv.addr, align 8
  call void @llvm.dbg.declare(metadata i8*** %argv.addr, metadata !23, metadata !21), !dbg !24
  call void @foo(), !dbg !25
  call void @bar(), !dbg !26
  call void @baz(), !dbg !27
  ret i32 0, !dbg !28
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nounwind uwtable
define void @foo() #0 !dbg !12 {
entry:
  ret void, !dbg !29
}

; Function Attrs: nounwind uwtable
define void @bar() #0 !dbg !15 {
entry:
  ret void, !dbg !30
}

; Function Attrs: nounwind uwtable
define void @baz() #0 !dbg !16 {
entry:
  ret void, !dbg !31
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!17, !18}
!llvm.ident = !{!19}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 3.9.0 ", isOptimized: false, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !3)
!1 = !DIFile(filename: "/usr/home/ck8418/dev/loom/build/test/Output/function-instrumentation.c.tmp.c", directory: "/usr/home/ck8418/dev/loom/build/test")
!2 = !{}
!3 = !{!4, !12, !15, !16}
!4 = distinct !DISubprogram(name: "main", scope: !5, file: !5, line: 40, type: !6, isLocal: false, isDefinition: true, scopeLine: 41, flags: DIFlagPrototyped, isOptimized: false, variables: !2)
!5 = !DIFile(filename: "/usr/home/ck8418/dev/loom/test/function-instrumentation.c", directory: "/usr/home/ck8418/dev/loom/build/test")
!6 = !DISubroutineType(types: !7)
!7 = !{!8, !8, !9}
!8 = !DIBasicType(name: "int", size: 32, align: 32, encoding: DW_ATE_signed)
!9 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !10, size: 64, align: 64)
!10 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64, align: 64)
!11 = !DIBasicType(name: "char", size: 8, align: 8, encoding: DW_ATE_signed_char)
!12 = distinct !DISubprogram(name: "foo", scope: !5, file: !5, line: 64, type: !13, isLocal: false, isDefinition: true, scopeLine: 65, flags: DIFlagPrototyped, isOptimized: false, variables: !2)
!13 = !DISubroutineType(types: !14)
!14 = !{null}
!15 = distinct !DISubprogram(name: "bar", scope: !5, file: !5, line: 76, type: !13, isLocal: false, isDefinition: true, scopeLine: 77, flags: DIFlagPrototyped, isOptimized: false, variables: !2)
!16 = distinct !DISubprogram(name: "baz", scope: !5, file: !5, line: 88, type: !13, isLocal: false, isDefinition: true, scopeLine: 89, flags: DIFlagPrototyped, isOptimized: false, variables: !2)
!17 = !{i32 2, !"Dwarf Version", i32 2}
!18 = !{i32 2, !"Debug Info Version", i32 3}
!19 = !{!"clang version 3.9.0 "}
!20 = !DILocalVariable(name: "argc", arg: 1, scope: !4, file: !5, line: 40, type: !8)
!21 = !DIExpression()
!22 = !DILocation(line: 40, column: 10, scope: !4)
!23 = !DILocalVariable(name: "argv", arg: 2, scope: !4, file: !5, line: 40, type: !9)
!24 = !DILocation(line: 40, column: 22, scope: !4)
!25 = !DILocation(line: 46, column: 2, scope: !4)
!26 = !DILocation(line: 50, column: 2, scope: !4)
!27 = !DILocation(line: 54, column: 2, scope: !4)
!28 = !DILocation(line: 58, column: 2, scope: !4)
!29 = !DILocation(line: 72, column: 1, scope: !12)
!30 = !DILocation(line: 84, column: 1, scope: !15)
!31 = !DILocation(line: 96, column: 1, scope: !16)
