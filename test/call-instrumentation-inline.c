/**
 * \file  call-instrumentation-inline.c
 * \brief Tests caller-context function instrumentation using InlineStrategy.
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

strategy: inline

logging: printf

hook_prefix: __test_hook

block_structure: true

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

// CHECK: define [[FOO_TYPE:i[0-9]+]] @foo(i32{{.*}}, float{{.*}}, double{{.*}})
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
	printf("Hello, world!\n");

	printf("First, we will call foo():\n");

	// We should instrument foo's call and return:

	// CHECK:        "[[PREFIX:__test_hook]]_call_foo:preamble":
	// CHECK-NEXT:   call [[PRINTF:i32 .* @printf]](i8* getelementptr {{[^)]+}}), [[FOO_INSTR_ARGS:.*]])
	// CHECK-OUTPUT: call foo: 1 2{{.*}} 3{{.*}}
	// CHECK-NEXT:   br label %"[[PREFIX]]_call_foo:end"

	// CHECK:        "[[PREFIX]]_call_foo:end":
	// CHECK-NEXT:   br label %[[TARGET:.*]]

	// CHECK:        [[TARGET]]:
	// CHECK-NEXT:   [[FOO_RET:%.*]] = call [[FOO_TYPE]] @foo(i32 1, float 2.{{[^,]+}}, double 3.{{[^)]+}})
	foo(1, 2, 3);

	// CHECK:        "[[PREFIX]]_return_foo:preamble":
	// CHECK-NEXT:   call [[PRINTF]](i8* getelementptr {{[^)]+}}), [[FOO_TYPE]] [[FOO_RET]], [[FOO_INSTR_ARGS]])
	// CHECK-OUTPUT: return foo: 1 1 2{{.*}} 3{{.*}}
	// CHECK-NEXT:   br label %"[[PREFIX]]_return_foo:end"

	// CHECK:        "[[PREFIX]]_return_foo:end":
	// CHECK-NEXT:   br label

	printf("Then bar():\n");

	// We should instrument bar's call but not return:
	// CHECK-OUTPUT: call bar: 4 "5"
	bar(4, "5");
	// CHECK-OUTPUT-NOT: return bar

	printf("And finally baz():\n");

	// We should not instrument the call to baz:
	// CHECK-OUTPUT-NOT: call baz
	baz();
	// CHECK-OUTPUT-NOT: return baz

	return 0;
}

#endif /* !POLICY_FILE */
