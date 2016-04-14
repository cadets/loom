; ModuleID = '/usr/home/ck8418/dev/loom/build/test/Output/static.c.tmp.ll'
source_filename = "/usr/home/ck8418/dev/loom/build/test/Output/static.c.tmp.ll"
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
  call void @llvm.dbg.declare(metadata i32* %argc.addr, metadata !18, metadata !19), !dbg !20
  store i8** %argv, i8*** %argv.addr, align 8
  call void @llvm.dbg.declare(metadata i8*** %argv.addr, metadata !21, metadata !19), !dbg !22
  call void @foo(), !dbg !23
  ret i32 0, !dbg !24
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

; Function Attrs: nounwind uwtable
define internal void @foo() #0 !dbg !12 {
entry:
  ret void, !dbg !25
}

attributes #0 = { nounwind uwtable "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readnone }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!15, !16}
!llvm.ident = !{!17}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "clang version 3.9.0 ", isOptimized: false, runtimeVersion: 0, emissionKind: 1, enums: !2, subprograms: !3)
!1 = !DIFile(filename: "/usr/home/ck8418/dev/loom/build/test/Output/static.c.tmp.c", directory: "/usr/home/ck8418/dev/loom/build/test")
!2 = !{}
!3 = !{!4, !12}
!4 = distinct !DISubprogram(name: "main", scope: !5, file: !5, line: 30, type: !6, isLocal: false, isDefinition: true, scopeLine: 31, flags: DIFlagPrototyped, isOptimized: false, variables: !2)
!5 = !DIFile(filename: "/usr/home/ck8418/dev/loom/test/static.c", directory: "/usr/home/ck8418/dev/loom/build/test")
!6 = !DISubroutineType(types: !7)
!7 = !{!8, !8, !9}
!8 = !DIBasicType(name: "int", size: 32, align: 32, encoding: DW_ATE_signed)
!9 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !10, size: 64, align: 64)
!10 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !11, size: 64, align: 64)
!11 = !DIBasicType(name: "char", size: 8, align: 8, encoding: DW_ATE_signed_char)
!12 = distinct !DISubprogram(name: "foo", scope: !5, file: !5, line: 42, type: !13, isLocal: true, isDefinition: true, scopeLine: 43, isOptimized: false, variables: !2)
!13 = !DISubroutineType(types: !14)
!14 = !{null}
!15 = !{i32 2, !"Dwarf Version", i32 2}
!16 = !{i32 2, !"Debug Info Version", i32 3}
!17 = !{!"clang version 3.9.0 "}
!18 = !DILocalVariable(name: "argc", arg: 1, scope: !4, file: !5, line: 30, type: !8)
!19 = !DIExpression()
!20 = !DILocation(line: 30, column: 10, scope: !4)
!21 = !DILocalVariable(name: "argv", arg: 2, scope: !4, file: !5, line: 30, type: !9)
!22 = !DILocation(line: 30, column: 22, scope: !4)
!23 = !DILocation(line: 35, column: 2, scope: !4)
!24 = !DILocation(line: 38, column: 2, scope: !4)
!25 = !DILocation(line: 49, column: 1, scope: !12)
