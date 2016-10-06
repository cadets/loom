/*
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

hook_prefix: TEST

logging: printf

block_structure: true

structures:
  - name: foo
    fields:
      - name: f_int
        operations: [ read, write ]

#else

struct foo {
	int		f_int;
	int		f_int2;
};

struct bar {
	int		b_int;
	double		b_double;
	struct foo	b_foo;
};


int
main(int argc, char *argv[])
{
	struct bar bars[] = {
		{ .b_foo = { .f_int = 99 } },
		{ .b_foo = { .f_int = 42 } },
	};

	// CHECK: [[F_INT_PTR:%.+]] = getelementptr inbounds %struct.foo, [[FOO:%.*]], i32 0, i32 0
	// CHECK: [[F_INT_NOW:%[0-9]+]] = load [[INT:i[0-9]+]], [[INT]]* [[F_INT_PTR]]
	// CHECK: call void @[[PREFIX:TEST]]_load_struct_foo_field_f_int([[FOO]], [[INT]] [[F_INT_NOW]]
	// CHECK: [[INC:%.+]] = add {{.*}}[[INT]] [[F_INT_NOW]], 1
	// CHECK: call void @[[PREFIX]]_store_struct_foo_field_f_int([[FOO]], [[INT]] [[INC]])

	// CHECK-OUTPUT: foo.f_int load: [[FOO:0x.*]] 42
	// CHECK-OUTPUT: foo.f_int store: [[FOO]] 43
	int offset = 1;
	(bars + offset)->b_foo.f_int++;

	return 0;
}

#endif /* !POLICY_FILE */

