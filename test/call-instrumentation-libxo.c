/**
 * \file  call-instrumentation-libxo.c
 * \brief Tests caller-context function instrumentation using libxo.
 *
 * Commands for llvm-lit:
 * RUN: %cpp -DPOLICY_FILE %s > %t.yaml
 * RUN: %cpp %s > %t.c
 * RUN: %clang %cflags -S -emit-llvm %cflags %t.c -o %t.ll
 * RUN: %loom -S %t.ll -loom-file %t.yaml -loom-logging=libxo -o %t.instr.ll
 * RUN: %filecheck -input-file %t.instr.ll %s
 * RUN: %llc -filetype=obj %t.instr.ll -o %t.instr.o
 * RUN: %clang %ldflags %t.instr.o -o %t.instr
 * RUN: %t.instr > %t.output
 * RUN: %filecheck -input-file %t.output %s -check-prefix CHECK-OUTPUT
 */

#if defined (POLICY_FILE)

hook_prefix: __test_hook

functions:
    - name: foo
      caller: [ entry, exit ]

    - name: bar
      caller: [ entry ]
      callee: [ exit ]

    - name: baz
      callee: [ entry, exit ]

#else

#include <stdio.h>
#include <stdlib.h>

#include <libxo/xo.h>

// CHECK: define [[FOO_TYPE:.*]] @foo(i32{{.*}}, float{{.*}}, double{{.*}})
int	foo(int x, float y, double z)
{
	printf("foo(%d, %f, %f\n", x, y, z);
	return x;
}

// CHECK: define [[BAR_TYPE:.*]] @bar(i32{{.*}}, i8*{{.*}})
float	bar(unsigned int i, const char *s)
{
	printf("bar(%d, \"%s\")\n", i, s);
	return i;
}

// CHECK: define double @baz()
double	baz(void)
{
	printf("baz()\n");
	return 0;
}

int
main(int argc, char *argv[])
{
	atexit(xo_finish_atexit);

	printf("Hello, world!\n");

	printf("First, we will call foo():\n");

	// We should instrument foo's call and return:
	// CHECK: call void @[[PREFIX:__test_hook]]_call_foo([[FOO_ARGS:.*]])
	// CHECK: [[FOO_RET:.*]] = call [[FOO_TYPE]] @foo([[FOO_ARGS]])
	// CHECK-OUTPUT: call foo: 1 2 3
	foo(1, 2, 3);
	// CHECK-OUTPUT: return foo: 1 1 2 3
	// CHECK: call void @[[PREFIX]]_return_foo([[FOO_TYPE]][[FOO_RET]], [[FOO_ARGS]])

	printf("Then bar():\n");

	// We should instrument bar's call but not return:
	// CHECK: call void @[[PREFIX]]_call_bar([[BAR_ARGS:.*]])
	// CHECK: call [[BAR_TYPE]] @bar([[BAR_ARGS]])
	// CHECK-OUTPUT: call bar: 4 "5"
	bar(4, "5");
	// CHECK-OUTPUT-NOT: return bar
	// CHECK-NOT: call void @[[PREFIX]]_return_bar

	printf("And finally baz():\n");

	// We should not instrument the call to baz:
	// CHECK-NOT: call {{.*}}_call_baz
	// CHECK-OUTPUT-NOT: call baz
	baz();
	// CHECK-OUTPUT-NOT: return baz
	// CHECK-NOT: call {{.*}}_return_baz

	return 0;
}

#endif /* !POLICY_FILE */
