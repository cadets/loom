/**
 * \file  field-instrumentation.c
 * \brief Tests instrumentation of structure field accesses.
 *
 * Commands for llvm-lit:
 * RUN: %cpp -DPOLICY_FILE %s > %t.yaml
 * RUN: %cpp %cflags %s > %t.c
 * RUN: %clang %cflags -S -emit-llvm %cflags %t.c -o %t.ll
 * RUN: %loom -S %t.ll -loom-file %t.yaml -loom-logging=printf -o %t.instr.ll
 * RUN: %filecheck -input-file %t.instr.ll %s
 * RUN: %llc -filetype=obj %t.instr.ll -o %t.instr.o
 * RUN: %clang %ldflags %t.instr.o -o %t.instr
 * RUN: %t.instr > %t.output
 * RUN: %filecheck -input-file %t.output %s -check-prefix CHECK-OUTPUT
 */

#if defined (POLICY_FILE)

hook_prefix: __test_hook

#else

#include <stdio.h>

struct foo {
	int		f_int;
	float		f_float;
	double		f_double;
	const char *	f_string;
};

struct bar {
	// TODO: struct foo	 b_foo;
	struct foo	*b_fooptr;
};


int
main(int argc, char *argv[])
{
	struct foo f = {
		.f_int = 42,
		.f_float = 3,
		.f_double = 3.1415926,
		.f_string = "Pi is exactly equal to 3!",
	};

	struct bar b = {
		//.b_foo = f,

		// CHECK: call void @[[PREFIX:__test_hook]]_store_bar_[[FOOPTR:.*]]([[BAR:.*]], [[FOO:.*]])
		// CHECK-OUTPUT: bar.[[FOOPTR:.*]] store: [[BAR:.*]] [[FOO:.*]]
		.b_fooptr = &f,
	};

	// CHECK: [[F_INT_PTR:%.+]] = getelementptr {{.*}}%struct.foo, [[FOO]]
	// CHECK: [[F_INT_NOW:%[0-9]+]] = load [[INT:i[0-9]+]], [[INT]]* [[F_INT_PTR]]
	// CHECK: call void @[[PREFIX]]_load_foo_[[F_INT:.*]]([[FOO]], [[INT]] [[F_INT_NOW]]
	// CHECK: [[INC:%.+]] = add {{.*}}[[INT]] [[F_INT_NOW]], 1
	// CHECK: call void @[[PREFIX]]_store_foo_[[F_INT]]([[FOO]], [[INT]] [[INC]])
	// CHECK-OUTPUT: foo.[[F_INT:.*]] load: [[FOO]] 42
	// CHECK-OUTPUT: foo.[[F_INT]] store: [[FOO]] 43
	f.f_int++;

	// CHECK: [[F_FLOAT_PTR:%.+]] = getelementptr {{.*}}%struct.foo, [[FOO]]
	// CHECK: [[F_FLOAT_NOW:%[0-9]+]] = load float, float* [[F_FLOAT_PTR]]
	// CHECK: call void @[[PREFIX]]_load_foo_[[F_FLOAT:.*]]([[FOO]], float [[F_FLOAT_NOW]]
	// CHECK: [[SUB:%.+]] = fsub {{.*}}float [[F_FLOAT_NOW]], 1
	// CHECK: call void @[[PREFIX]]_store_foo_[[F_FLOAT]]([[FOO]], float [[SUB]])
	// CHECK-OUTPUT: foo.[[F_FLOAT:.*]] load: [[FOO]] 3
	// CHECK-OUTPUT: foo.[[F_FLOAT]] store: [[FOO]] 2
	f.f_float -= 1;

	// CHECK: [[F_DOUBLE_PTR:%.+]] = getelementptr {{.*}}%struct.foo, [[FOO]]
	// CHECK: call void @[[PREFIX]]_store_foo_[[F_DOUBLE:field[0-9]+]]([[FOO]], double -1
	// CHECK: store double -1{{.*}}, double* [[F_DOUBLE_PTR]]
	// CHECK-OUTPUT: foo.[[F_DOUBLE:field[0-9]+]] store: [[FOO]] -1
	f.f_double = -1;

	// CHECK: [[MESSAGE_PTR:%.+]] = getelementptr {{.*}}%struct.foo, [[FOO]]
	// CHECK: [[MESSAGE:%.+]] = load i8*, i8** [[MESSAGE_PTR]]
	// CHECK: call void @[[PREFIX]]_load_foo_field{{[0-9]+}}([[FOO]], i8* [[MESSAGE]])
	// CHECK-OUTPUT: foo.field{{[0-9]+}} load: [[FOO]] "[[MESSAGE:.*]]"
	// CHECK-OUTPUT: message: '[[MESSAGE]]'
	const char *message = f.f_string;
	printf("message: '%s'\n", message);

	//TODO: b.b_foo.f_int = 0;

	// CHECK: [[FOOPTR_PTR:%.+]] = getelementptr {{.*}}%struct.bar, [[BAR]]
	// CHECK: [[FOOPTR_VAL:%.+]] = load %struct.foo*, %struct.foo** [[FOOPTR_PTR]]
	// CHECK: call void @[[PREFIX]]_load_bar_[[FOOPTR]]([[BAR]], %struct.foo* [[FOOPTR_VAL]])
	// CHECK-OUTPUT: bar.[[FOOPTR]] load: [[BAR]] [[FOO]]

	// CHECK: call void @[[PREFIX]]_store_foo_[[FOOPTR]](%struct.foo* [[FOOPTR_VAL]], [[INT]] 0)
	// CHECK-OUTPUT: foo.field{{[0-9]+}} store: [[FOO]] 0
	b.b_fooptr->f_int = 0;

	return 0;
}

#endif /* !POLICY_FILE */