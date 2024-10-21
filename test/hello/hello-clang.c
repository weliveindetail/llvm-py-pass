// RUN: clang -O1 -fpass-plugin=%plugin -emit-llvm -S %s -o - 2>&1 1>/dev/null | FileCheck %s
// CHECK: hello 12345

// clang-format off

int main() { return 0; }
