/**
 * \file  ktrace-userspace-libnv.c
 * \brief Tests userspace ktrace instrumentation with libnv serialization.
 *
 * Commands for llvm-lit:
 * RUN: %cpp -DPOLICY_FILE %s > %t.yaml
 * RUN: %cpp %cflags %s > %t.c
 * RUN: %clang %cflags -S -emit-llvm %cflags %t.c -o %t.ll
 * RUN: %loom -S %t.ll -loom-file %t.yaml -loom-ktrace=utrace -loom-serialization=nv -o %t.instr.ll
 * RUN: %filecheck -input-file %t.instr.ll %s
 */

#if defined (POLICY_FILE)

hook_prefix: __ktrace_test

functions:
    - name: foo
      caller: [ entry ]

#else

#include <stdio.h>

// CHECK: define [[FOO_TYPE:i[0-9]+]] @foo(i32{{.*}}, float{{.*}}, double{{.*}})
int	foo(int x, float y, double z)
{
	return x;
}

// CHECK:       define{{.*}} void @__ktrace_test_call_foo
// CHECK-NEXT:  preamble:
// CHECK:       [[LIST:%.*]] = call %nvlist_t* @nvlist_create
// CHECK:       [[BUFFER:%.*]] = call i8* @nvlist_pack(%nvlist_t* [[LIST]], i64* [[LENPTR:%.*]])
// CHECK-DAG:   [[LEN:%.*]] = load i64, i64* [[LENPTR]]
// CHECK-DAG:   call void @nvlist_destroy(%nvlist_t* [[LIST]])
// CHECK:       call i32 @utrace(i8* [[BUFFER]], i64 [[LEN]])

int
main(int argc, char *argv[])
{
	foo(1, 2, 3);

	return 0;
}

#endif /* !POLICY_FILE */
