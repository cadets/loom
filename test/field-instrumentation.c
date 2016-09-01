/**
 * \file  field-instrumentation.c
 * \brief Tests instrumentation of structure field accesses.
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

logging: printf

structures:
  - name: foo
    fields:
      - name: f_int
        operations: [ read, write ]
      - name: f_float
        operations: [ read, write ]
      - name: f_double
        operations: [ read, write ]
      - name: f_string
        operations: [ read, write ]

  - name: bar
    fields:
      - name: b_fooptr
        operations: [ read, write ]

#else

#include <stdio.h>

struct foo {
	int		f_int;
	float		f_float;
	double		f_double;
	const char *	f_string;
	int		f_ignored;
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

		// CHECK: call void @[[PREFIX:__test_hook]]_store_struct_bar_field_b_fooptr([[BAR:.*]], [[FOO:.*]])
		// CHECK-OUTPUT: bar.b_fooptr store: [[BAR:.*]] [[FOO:.*]]
		.b_fooptr = &f,
	};

	// CHECK: [[F_INT_PTR:%.+]] = getelementptr {{.*}}%struct.foo, [[FOO]]
	// CHECK: [[F_INT_NOW:%[0-9]+]] = load [[INT:i[0-9]+]], [[INT]]* [[F_INT_PTR]]
	// CHECK: call void @[[PREFIX]]_load_struct_foo_field_f_int([[FOO]], [[INT]] [[F_INT_NOW]]
	// CHECK: [[INC:%.+]] = add {{.*}}[[INT]] [[F_INT_NOW]], 1
	// CHECK: call void @[[PREFIX]]_store_struct_foo_field_f_int([[FOO]], [[INT]] [[INC]])
	// CHECK-OUTPUT: foo.f_int load: [[FOO]] 42
	// CHECK-OUTPUT: foo.f_int store: [[FOO]] 43
	f.f_int++;

	// CHECK: [[F_FLOAT_PTR:%.+]] = getelementptr {{.*}}%struct.foo, [[FOO]]
	// CHECK: [[F_FLOAT_NOW:%[0-9]+]] = load float, float* [[F_FLOAT_PTR]]
	// CHECK: call void @[[PREFIX]]_load_struct_foo_field_f_float([[FOO]], float [[F_FLOAT_NOW]]
	// CHECK: [[SUB:%.+]] = fsub {{.*}}float [[F_FLOAT_NOW]], 1
	// CHECK: call void @[[PREFIX]]_store_struct_foo_field_f_float([[FOO]], float [[SUB]])
	// CHECK-OUTPUT: foo.f_float load: [[FOO]] 3
	// CHECK-OUTPUT: foo.f_float store: [[FOO]] 2
	f.f_float -= 1;

	// CHECK: [[F_DOUBLE_PTR:%.+]] = getelementptr {{.*}}%struct.foo, [[FOO]]
	// CHECK: call void @[[PREFIX]]_store_struct_foo_field_f_double([[FOO]], double -1
	// CHECK: store double -1{{.*}}, double* [[F_DOUBLE_PTR]]
	// CHECK-OUTPUT: foo.f_double store: [[FOO]] -1
	f.f_double = -1;

	// CHECK: [[F_IGNORED_PTR:%.+]] = getelementptr {{.*}}%struct.foo, [[FOO]]
	// CHECK-NOT: call void @[[PREFIX]]_store_{{.*}}([[FOO]], i32 99
	// CHECK: store i32 99, i32* [[F_IGNORED_PTR]]
	f.f_ignored = 99;

	// CHECK: [[MESSAGE_PTR:%.+]] = getelementptr {{.*}}%struct.foo, [[FOO]]
	// CHECK: [[MESSAGE:%.+]] = load i8*, i8** [[MESSAGE_PTR]]
	// CHECK: call void @[[PREFIX]]_load_struct_foo_field_f_string([[FOO]], i8* [[MESSAGE]])
	// CHECK-OUTPUT: foo.f_string load: [[FOO]] "[[MESSAGE:.*]]"
	// CHECK-OUTPUT: message: '[[MESSAGE]]'
	const char *message = f.f_string;
	printf("message: '%s'\n", message);

	//TODO: b.b_foo.f_int = 0;

	// CHECK: [[FOOPTR_PTR:%.+]] = getelementptr {{.*}}%struct.bar, [[BAR]]
	// CHECK: [[FOOPTR_VAL:%.+]] = load %struct.foo*, %struct.foo** [[FOOPTR_PTR]]
	// CHECK: call void @[[PREFIX]]_load_struct_bar_field_b_fooptr([[BAR]], %struct.foo* [[FOOPTR_VAL]])
	// CHECK-OUTPUT: bar.b_fooptr load: [[BAR]] [[FOO]]

	// CHECK: call void @[[PREFIX]]_store_struct_foo_field_f_int(%struct.foo* [[FOOPTR_VAL]], [[INT]] 123)
	// CHECK-OUTPUT: foo.f_int store: [[FOO]] 123
	b.b_fooptr->f_int = 123;

	return 0;
}

#endif /* !POLICY_FILE */
