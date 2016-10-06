/**
 * \file  ktrace-userspace-libnv-run.c
 * \brief Actually run utrace instrumentation with libnv serialization.
 *
 * FreeBSD <11 doesn't provide the libnv we're looking for in base or ports:
 * XFAIL: darwin, freebsd9, freebsd10, linux
 *
 * Commands for llvm-lit:
 * RUN: %cpp -DPOLICY_FILE %s > %t.yaml
 * RUN: %cpp %cflags %s > %t.c
 * RUN: %clang -target x86_64-unknown-freebsd %cflags -S -emit-llvm %cflags %t.c -o %t.ll
 * RUN: %loom -S %t.ll -loom-file %t.yaml -loom-nv-debug -o %t.instr.ll
 * RUN: %clang -lnv %t.instr.ll -o %t.instr
 * RUN: %t.instr 2> %t.instr.out
 * RUN: %filecheck -input-file %t.instr.out %s
 */

#if defined (POLICY_FILE)

hook_prefix: __ktrace_test

ktrace: utrace

serialization: nv

block_structure: true

functions:
    - name: foo
      caller: [ exit ]

#else

#include <stdio.h>

int	foo(int x, float y, double z)
{
	return x;
}

// CHECK:      name (STRING): [__ktrace_test_return_foo]
// CHECK-NEXT: description (STRING): [return foo:]
// CHECK-NEXT: values (NVLIST):
// CHECK-NEXT:   (NUMBER): {{.*}} (1) (0x1)
// CHECK-NEXT:   (NUMBER): {{.*}} (1) (0x1)

int
main(int argc, char *argv[])
{
	foo(1, 2, 3);

	return 0;
}

#endif /* !POLICY_FILE */
