/**
 * \file  ktrace-kernel.c
 * \brief Tests kernel-mode ktrace instrumentation.
 *
 * Commands for llvm-lit:
 * RUN: %cpp -DPOLICY_FILE %s > %t.yaml
 * RUN: %cpp %cflags %s > %t.c
 * RUN: %clang %cflags -S -emit-llvm %cflags %t.c -o %t.ll
 * RUN: %loom -S %t.ll -loom-file %t.yaml -o %t.instr.ll
 * RUN: %filecheck -input-file %t.instr.ll %s
 */

#if defined (POLICY_FILE)

hook_prefix: __ktrace_test

ktrace: kernel

block_structure: true

functions:
    - name: foo
      caller: [ entry ]

#else

#include <stdio.h>

// CHECK: define{{.*}} [[FOO_TYPE:i[0-9]+]] @foo(i32{{.*}}, float{{.*}}, double{{.*}})
int	foo(int x, float y, double z)
{
	return x;
}

// CHECK:       define{{.*}} void @__ktrace_test_call_foo
// CHECK-NEXT:  preamble:
// CHECK:       call void @ktrstruct

int
main(int argc, char *argv[])
{
	foo(1, 2, 3);

	return 0;
}

#endif /* !POLICY_FILE */
