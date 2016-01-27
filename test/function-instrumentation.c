/*
 * \file  function-instrumentation.c
 * \brief Tests caller-context function instrumentation.
 *
 * Commands for llvm-lit:
 * RUN: %cpp -DPOLICY_FILE %s > %t.yaml
 * RUN: %cpp %s > %t.c
 * RUN: %clang -S -emit-llvm %cflags %t.c -o %t.ll
 * RUN: %opt -S %t.ll -loom-file %t.yaml -o %t.instr.ll
 * RUN: %filecheck -input-file %t.instr.ll %s
 *
 * XFAIL: *
 */

#if defined (POLICY_FILE)

hook_prefix: __test_hook

functions:
    - name: main
      callee: [ entry, exit ]

    - name: foo
      caller: []

    - name: bar
      callee: [ exit ]

    - name: baz
      callee: [ entry, exit ]
      caller: []

#else /* !defined(POLICY_FILE) */

void	foo(void);
void	bar(void);
void	baz(void);

int
main(int argc, char *argv[])
{
	// CHECK: define [[INT:i[3264]+]] @main([[INT]] %argc, i8** %argv)
	// CHECK: call void @[[PREFIX:__test_hook]]_enter_main([[INT]] %argc, i8** %argv)

	// CHECK-NOT: call void @[[PREFIX]]_call_foo
	foo();
	// CHECK-NOT: call void @[[PREFIX]]_return_foo

	// CHECK-NOT: call void @[[PREFIX]]_call_bar
	bar();
	// CHECK-NOT: call void @[[PREFIX]]_return_bar

	// CHECK-NOT: call void @[[PREFIX]]_call_baz
	baz();
	// CHECK-NOT: call void @[[PREFIX]]_return_baz

	// CHECK: call void @[[PREFIX:__test_hook]]_leave_main([[INT]] %argc, i8** %argv, i32 0)
	return 0;
}


// CHECK: define void @foo()
void
foo()
{
	/*
	 * We should not generate any caller-side instrumentation here:
	 *
	 * CHECK-NOT: call void @[[PREFIX]]_enter_foo
	 * CHECK-NOT: call void @[[PREFIX]]_leave_foo
	 */
}

// CHECK: define void @bar()
void
bar()
{
	/*
	 * We ought to generate instrumentation for return but not entry:
	 *
	 * CHECK-NOT: call void @[[PREFIX]]_enter_bar
	 * CHECK: call void @[[PREFIX]]_leave_bar
	 */
}

// CHECK: define void @baz()
void
baz()
{
	/*
	 * We ought to generate instrumentation for entry and return:
	 *
	 * CHECK: call void @[[PREFIX]]_enter_baz
	 * CHECK: call void @[[PREFIX]]_leave_baz
	 */
}

#endif /* !defined(POLICY_FILE) */
