/*
 * \file  call-instrumentation.c
 * \brief Tests caller-context function instrumentation.
 *
 * Commands for llvm-lit:
 * RUN: %cpp -DPOLICY_FILE %s > %t.yaml
 * RUN: %cpp %s > %t.c
 * RUN: %clang -S -emit-llvm %cflags %t.c -o %t.ll
 * RUN: %loom -loom -S %t.ll -loom-file %t.yaml -o %t.instr.ll
 * RUN: %filecheck -input-file %t.instr.ll %s
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

// CHECK: define void @foo()
void	foo(void) {}

// CHECK: define void @bar()
void	bar(void) {}

// CHECK: define void @baz()
void	baz(void) {}

int
main(int argc, char *argv[])
{
	// We should instrument foo's call and return:
	// CHECK: call void @[[PREFIX:__test_hook]]_call_foo
	foo();
	// CHECK: call void @[[PREFIX]]_return_foo

	// We should instrument bar's call but not return:
	// CHECK: call void @[[PREFIX]]_call_bar
	bar();
	// CHECK-NOT: call void @[[PREFIX]]_return_bar

	// We should not instrument the call to baz:
	// CHECK-NOT: call void @[[PREFIX]]_call_baz
	baz();
	// CHECK-NOT: call void @[[PREFIX]]_return_baz

	return 0;
}

#endif /* !POLICY_FILE */
