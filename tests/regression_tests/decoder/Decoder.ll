; RUN: make -C %S all
; RUN: %rosetta --input %S/Decoder.elf --output /dev/null --stop-after decoder --dump-input-instructions=true > %t
; RUN: FileCheck %s < %t

; CHECK: mov %rbx, %rax
; CHECK-NEXT: mov %rdx, %rcx
; CHECK-NEXT: mov %r9, %r8
; CHECK-NEXT: mov %r11, %r10
; CHECK-NEXT: mov %r13, %r12
; CHECK-NEXT: mov %r15, %r14
; CHECK-NEXT: mov %bl, %al
; CHECK-NEXT: mov %bx, %ax
; CHECK-NEXT: mov %ebx, %eax
; CHECK-NEXT: mov $0x123456789ABCDEF0, %rax
; CHECK-NEXT: mov $0x12345678, %eax
; CHECK-NEXT: mov (%rax), %rcx
; CHECK-NEXT: mov 0x10(%rax), %rdx
; CHECK-NEXT: mov 0x20(%rax,%rbx,2), %r8
; CHECK-NEXT: mov -0x40(%r12,%r13,8), %r9
; CHECK-NEXT: mov %rax, 0x1234(%rdi,%rsi,4)
; CHECK-NEXT: mov %dl, (%rcx)
; CHECK-NEXT: mov %dx, 0x01(%rcx)
; CHECK-NEXT: mov %edx, 0x04(%rcx)
; CHECK-NEXT: mov %rdx, 0x08(%rcx)
; CHECK-NEXT: add %rbx, %rax
; CHECK-NEXT: adc %rdx, %rcx
; CHECK-NEXT: sub %rdi, %rsi
; CHECK-NEXT: sbb %r9, %r8
; CHECK-NEXT: inc %r10
; CHECK-NEXT: dec %r11
; CHECK-NEXT: neg %r12
; CHECK-NEXT: and %r14, %r13
; CHECK-NEXT: or %rax, %r15
; CHECK-NEXT: xor %rbx, %rbx
; CHECK-NEXT: cmp %rdx, %rcx
; CHECK-NEXT: test %rsi, %rsi
; CHECK-NEXT: add $0x7F, %rbx
; CHECK-NEXT: add $-0x01, %rcx
; CHECK-NEXT: sub $0x1234, %rdx
; CHECK-NEXT: and $0xFF, %rsi
; CHECK-NEXT: or $0x8000, %rdi
; CHECK-NEXT: xor $0x55, %r8
; CHECK-NEXT: shl $0x01, %rax
; CHECK-NEXT: shr $0x01, %rbx
; CHECK-NEXT: sar $0x01, %rcx
; CHECK-NEXT: rol $0x01, %rdx
; CHECK-NEXT: ror $0x01, %rsi
; CHECK-NEXT: imul %r8, %rdi
; CHECK-NEXT: imul $0x07, %r10, %r9
; CHECK-NEXT: not %r14
; CHECK-NEXT: push %rax
; CHECK-NEXT: push %rbx
; CHECK-NEXT: pop %rcx
; CHECK-NEXT: pop %rdx
; CHECK-NEXT: nop
; CHECK-NEXT: ret
; RUN: make clean
