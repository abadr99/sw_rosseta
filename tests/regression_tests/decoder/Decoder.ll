; RUN: make -C %S all
; RUN: %rosetta --input %S/Decoder.elf --output /dev/null --stop-after decoder --dump-input-instructions=true > %t
; RUN: FileCheck %s < %t

; CHECK: mov rax, rbx
; CHECK-NEXT: mov rcx, rdx
; CHECK-NEXT: mov r8, r9
; CHECK-NEXT: mov r10, r11
; CHECK-NEXT: mov r12, r13
; CHECK-NEXT: mov r14, r15
; CHECK-NEXT: mov al, bl
; CHECK-NEXT: mov ax, bx
; CHECK-NEXT: mov eax, ebx
; CHECK-NEXT: mov rax, 0x123456789ABCDEF0
; CHECK-NEXT: mov eax, 0x12345678
; CHECK-NEXT: mov rcx, [rax]
; CHECK-NEXT: mov rdx, [rax+0x10]
; CHECK-NEXT: mov r8, [rax+rbx*2+0x20]
; CHECK-NEXT: mov r9, [r12+r13*8-0x40]
; CHECK-NEXT: mov [rdi+rsi*4+0x1234], rax
; CHECK-NEXT: mov [rcx], dl
; CHECK-NEXT: mov [rcx+0x01], dx
; CHECK-NEXT: mov [rcx+0x04], edx
; CHECK-NEXT: mov [rcx+0x08], rdx
; CHECK-NEXT: add rax, rbx
; CHECK-NEXT: adc rcx, rdx
; CHECK-NEXT: sub rsi, rdi
; CHECK-NEXT: sbb r8, r9
; CHECK-NEXT: inc r10
; CHECK-NEXT: dec r11
; CHECK-NEXT: neg r12
; CHECK-NEXT: and r13, r14
; CHECK-NEXT: or r15, rax
; CHECK-NEXT: xor rbx, rbx
; CHECK-NEXT: cmp rcx, rdx
; CHECK-NEXT: test rsi, rsi
; CHECK-NEXT: add rbx, 0x7F
; CHECK-NEXT: add rcx, 0xFFFFFFFFFFFFFFFF
; CHECK-NEXT: sub rdx, 0x1234
; CHECK-NEXT: and rsi, 0xFF
; CHECK-NEXT: or rdi, 0x8000
; CHECK-NEXT: xor r8, 0x55
; CHECK-NEXT: shl rax, 0x01
; CHECK-NEXT: shr rbx, 0x01
; CHECK-NEXT: sar rcx, 0x01
; CHECK-NEXT: rol rdx, 0x01
; CHECK-NEXT: ror rsi, 0x01
; CHECK-NEXT: imul rdi, r8
; CHECK-NEXT: imul r9, r10, 0x07
; CHECK-NEXT: not r14
; CHECK-NEXT: push rax
; CHECK-NEXT: push rbx
; CHECK-NEXT: pop rcx
; CHECK-NEXT: pop rdx
; CHECK-NEXT: nop
; CHECK-NEXT: ret
; RUN: make clean