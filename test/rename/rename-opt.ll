; Check that function names change in IR 
;
; RUN: opt -load-pass-plugin=%plugin -passes=llvm-py-pass -S %s -o - | FileCheck %s
;
; CHECK-NOT: foo
; CHECK-NOT: bar
; CHECK: 2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae
; CHECK: fcde2b2edba56bf408601fb721fe9b5c338d10ee429ea04fae5511b68fbf8fb9
; CHECK: main

define i32 @foo() {
  ret i32 42
}

define i32 @bar() {
  ret i32 0
}

define i32 @main(i32 noundef %0, i8** noundef %1) {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i8**, align 8
  store i32 0, i32* %3, align 4
  store i32 %0, i32* %4, align 4
  store i8** %1, i8*** %5, align 8
  %6 = load i32, i32* %4, align 4
  %7 = srem i32 %6, 2
  %8 = icmp ne i32 %7, 0
  br i1 %8, label %9, label %11

9:
  %10 = call i32 @foo()
  br label %13

11:
  %12 = call i32 @bar()
  br label %13

13:
  %14 = phi i32 [ %10, %9 ], [ %12, %11 ]
  ret i32 %14
}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 14, i32 5]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 7, !"PIC Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 2}
