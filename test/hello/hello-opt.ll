; RUN: opt -load-pass-plugin=%plugin -passes=llvm-py-pass -S %s -o - 2>&1 1>/dev/null | FileCheck %s
; CHECK: hello 12345

define i32 @main() {
  ret i32 0
}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 14, i32 5]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 7, !"PIC Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 2}
