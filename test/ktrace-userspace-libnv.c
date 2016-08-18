/**
 * \file  ktrace-userspace-libnv.c
 * \brief Tests userspace ktrace instrumentation with libnv serialization.
 *
 * FreeBSD <11 doesn't provide the libnv we're looking for in base or ports:
 * XFAIL: freebsd9, freebsd10
 *
 * Commands for llvm-lit:
 * RUN: %cpp -DPOLICY_FILE %s > %t.yaml
 * RUN: %cpp %cflags %s > %t.c
 * RUN: %clang %cflags -S -emit-llvm %cflags %t.c -o %t.ll
 * RUN: %loom -S %t.ll -loom-file %t.yaml -loom-serialization=nv -loom-nv-debug -o %t.instr.ll
 * RUN: %filecheck -input-file %t.instr.ll %s
 * RUN: %clang -lnv %t.instr.ll -o %t.instr
 * RUN: %t.instr 2> %t.instr.out
 * RUN: %filecheck -input-file %t.instr.out -check-prefix=CHECK-OUTPUT %s
 */

#if defined (POLICY_FILE)

hook_prefix: __ktrace_test

ktrace: utrace

functions:
    - name: foo
      caller: [ exit ]

#else

#include <stdio.h>

// CHECK: define [[FOO_TYPE:i[0-9]+]] @foo(i32{{.*}}, float{{.*}}, double{{.*}})
int	foo(int x, float y, double z)
{
	return x;
}

// CHECK:       define{{.*}} void @__ktrace_test_return_foo
// CHECK-NEXT:  preamble:
// CHECK:       [[LIST:%.*]] = call %nvlist_t* @nvlist_create
// CHECK:       [[BUFFER:%.*]] = call i8* @nvlist_pack(%nvlist_t* [[LIST]], i64* [[LENPTR:%.*]])
// CHECK-DAG:   [[LEN:%.*]] = load i64, i64* [[LENPTR]]
// CHECK-DAG:   call void @nvlist_destroy(%nvlist_t* [[LIST]])
// CHECK:       call i32 @utrace(i8* [[BUFFER]], i64 [[LEN]])

// CHECK-OUTPUT:      name (STRING): [__ktrace_test_return_foo]
// CHECK-OUTPUT-NEXT: description (STRING): [return foo:]
// CHECK-OUTPUT-NEXT: values (NVLIST):
// CHECK-OUTPUT-NEXT:   (NUMBER): {{.*}} (1) (0x1)
// CHECK-OUTPUT-NEXT:   (NUMBER): {{.*}} (1) (0x1)

int
main(int argc, char *argv[])
{
	foo(1, 2, 3);

	return 0;
}

#endif /* !POLICY_FILE */
