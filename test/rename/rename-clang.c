// Check that symbol names change and the executable still works (and errors out with even argc)

// RUN: clang -O1 -fpass-plugin=%plugin %exeflags %s -o %t_exe
// RUN: llvm-nm %t_exe | FileCheck %s
//
// CHECK-NOT: foo
// CHECK-NOT: bar
// CHECK: 2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae
// CHECK: fcde2b2edba56bf408601fb721fe9b5c338d10ee429ea04fae5511b68fbf8fb9
// CHECK: main

// RUN: %t_exe
// RUN: not %t_exe 1

// clang-format off

int foo() { return 42; }
int bar() { return 0; }
int main(int argc, char *argv[]) {
  return (argc % 2 == 0) ? foo() : bar();
}
