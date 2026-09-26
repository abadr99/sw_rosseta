; RUN: make -C %S all
; RUN: %rosetta --input %S/Factorial.elf --output /dev/null --stop-after ssa --dump-ssa true > %t
; RUN: FileCheck %s < %t
; RUN: make -C %S clean

; Factorial SSA regression test.

; CHECK: {{.*}}const{{.*}}4
; CHECK: {{.*}}const{{.*}}1
; CHECK: {{.*}}sle{{.*}}
; CHECK: {{.*}}condbr{{.*}}

; CHECK: {{.*}}phi{{.*}}
; CHECK: {{.*}}mul{{.*}}
; CHECK: {{.*}}sub{{.*}}
; CHECK: {{.*}}sgt{{.*}}
; CHECK: {{.*}}condbr{{.*}}

; CHECK: {{.*}}load{{.*}}
; CHECK: {{.*}}indirectbr{{.*}}
