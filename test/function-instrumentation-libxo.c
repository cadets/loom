/**
 * \file  function-instrumentation-libxo.c
 * \brief Tests callee-context function instrumentation using libxo.
 *
 * Commands for llvm-lit:
 * RUN: %cpp -DPOLICY_FILE %s > %t.yaml
 * RUN: %cpp %cflags %s > %t.c
 * RUN: %clang %cflags -S -emit-llvm %cflags %t.c -o %t.ll
 * RUN: %loom -S %t.ll -loom-file %t.yaml -o %t.instr.ll
 * RUN: %filecheck -input-file %t.instr.ll %s
 * RUN: %llc -filetype=obj %t.instr.ll -o %t.instr.o
 * RUN: %clang %ldflags %t.instr.o -o %t.instr
 * RUN: %t.instr > %t.output
 * RUN: %filecheck -input-file %t.output %s -check-prefix CHECK-OUTPUT
 */

#if defined (POLICY_FILE)

hook_prefix: __test_hook

logging: xo

block_structure: true

functions:
    - name: foo
      callee: [ entry, exit ]

    - name: bar
      callee: [ entry ]

    - name: baz

#else

#include <stdio.h>
#include <stdlib.h>

#include <libxo/xo.h>

// CHECK: define{{.*}} [[FOO_TYPE:.*]] @foo([[INT:i[0-9]+]]{{.*}}, float{{.*}}, double{{.*}})
int	foo(int x, float y, double z)
{
	// CHECK: call void @[[PREFIX:__test_hook]]_enter_foo([[FOO_ARGS:.*]])
	printf("foo(%d, %g, %g)\n", x, y, z);
	return x;
	// CHECK: call void @[[PREFIX]]_leave_foo([[FOO_TYPE]] [[RETVAL:.*]], [[FOO_ARGS]])
	// CHECK-NEXT: ret [[FOO_TYPE]] [[RETVAL]]
}

// CHECK: define{{.*}} [[BAR_TYPE:.*]] @bar([[INT]]{{.*}}, i8*{{.*}})
float	bar(unsigned int i, const char *s)
{
	// CHECK: call void @[[PREFIX]]_enter_bar([[INT]] {{%.*}}, i8* {{%.*}})
	printf("bar(%d, \"%s\")\n", i, s);
	// CHECK-NOT: call void @[[PREFIX]]_leave_bar
	return i;
}

// CHECK: define{{.*}} double @baz()
double	baz(void)
{
	// CHECK-NOT: call void @[[PREFIX]]_enter_baz
	printf("baz()\n");
	// CHECK-NOT: call void @[[PREFIX]]_leave_baz
	return 0;
}

int
main(int argc, char *argv[])
{
	atexit(xo_finish_atexit);

	printf("Hello, world!\n");

	printf("First, we will call foo():\n");
	foo(1, 2, 3);
	// CHECK-OUTPUT: enter foo: 1 2{{(.0+)?}} 3{{(.0+)?}}
	// CHECK-OUTPUT: leave foo: 1 1 2{{(.0+)?}} 3{{(.0+)?}}

	printf("Then bar():\n");
	bar(4, "5");
	// CHECK-OUTPUT: enter bar: 4 0x{{[0-9a-z]+}}
	// CHECK-OUTPUT-NOT: leave bar

	printf("And finally baz():\n");
	baz();
	// CHECK-OUTPUT-NOT: enter baz
	// CHECK-OUTPUT-NOT: leave baz


	return 0;
}

#endif /* !POLICY_FILE */
