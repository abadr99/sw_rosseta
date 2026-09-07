; RUN: make -C %S all
; RUN: %rosetta --input %S/control_flow.elf --output /dev/null --stop-after decoder --dump-input-instructions=true > %t
; RUN: FileCheck %s < %t

; CHECK: cmp rax, rbx
; CHECK-NEXT: jz
; CHECK-NEXT: jnz
; CHECK-NEXT: jl
; CHECK-NEXT: jle
; CHECK-NEXT: jnle
; CHECK-NEXT: jnl
; CHECK-NEXT: jnbe
; CHECK-NEXT: jnb
; CHECK-NEXT: jb
; CHECK-NEXT: jbe
; CHECK-NEXT: jz
; CHECK-NEXT: jnz
; CHECK-NEXT: call
; CHECK-NEXT: ret
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: nop
; CHECK-NEXT: push rax
; CHECK-NEXT: pop rbx
; CHECK-NEXT: ret