# RUN: printf '%%s\n' \
# RUN: '.intel_syntax noprefix' \
# RUN: '.global _start' \
# RUN: '_start:' \
# RUN: '  mov rax, rbx' \
# RUN: '  mov rcx, rdx' \
# RUN: '  mov r8, r9' \
# RUN: '  mov r10, r11' \
# RUN: '  mov r12, r13' \
# RUN: '  mov r14, r15' \
# RUN: '  mov al, bl' \
# RUN: '  mov ax, bx' \
# RUN: '  mov eax, ebx' \
# RUN: '  mov rax, 0x123456789abcdef0' \
# RUN: '  mov eax, 0x12345678' \
# RUN: '  mov rcx, [rax]' \
# RUN: '  mov rdx, [rax + 0x10]' \
# RUN: '  mov r8, [rax + rbx*2 + 0x20]' \
# RUN: '  mov r9, [r12 + r13*8 - 0x40]' \
# RUN: '  mov [rdi + rsi*4 + 0x1234], rax' \
# RUN: '  mov byte ptr [rcx], dl' \
# RUN: '  mov word ptr [rcx+1], dx' \
# RUN: '  mov dword ptr [rcx+4], edx' \
# RUN: '  mov qword ptr [rcx+8], rdx' \
# RUN: '  add rax, rbx' \
# RUN: '  adc rcx, rdx' \
# RUN: '  sub rsi, rdi' \
# RUN: '  sbb r8, r9' \
# RUN: '  inc r10' \
# RUN: '  dec r11' \
# RUN: '  neg r12' \
# RUN: '  and r13, r14' \
# RUN: '  or r15, rax' \
# RUN: '  xor rbx, rbx' \
# RUN: '  cmp rcx, rdx' \
# RUN: '  test rsi, rsi' \
# RUN: '  add rbx, 0x7f' \
# RUN: '  add rcx, -1' \
# RUN: '  sub rdx, 0x1234' \
# RUN: '  and rsi, 0xff' \
# RUN: '  or rdi, 0x8000' \
# RUN: '  xor r8, 0x55' \
# RUN: '  shl rax, 1' \
# RUN: '  shr rbx, 1' \
# RUN: '  sar rcx, 1' \
# RUN: '  rol rdx, 1' \
# RUN: '  ror rsi, 1' \
# RUN: '  imul rdi, r8' \
# RUN: '  imul r9, r10, 7' \
# RUN: '  not r14' \
# RUN: '  push rax' \
# RUN: '  push rbx' \
# RUN: '  pop rcx' \
# RUN: '  pop rdx' \
# RUN: '  nop' \
# RUN: '  ret' \
# RUN: > %t.s
# RUN: gcc -nostdlib -Wl,--build-id=none %t.s -o %t.elf
# RUN: %regression_driver %t.elf | FileCheck %s

# Comprehensive decoder regression.

# CHECK: mov rax, rbx
# CHECK-NEXT: mov rcx, rdx
# CHECK-NEXT: mov r8, r9
# CHECK-NEXT: mov r10, r11
# CHECK-NEXT: mov r12, r13
# CHECK-NEXT: mov r14, r15
# CHECK-NEXT: mov al, bl
# CHECK-NEXT: mov ax, bx
# CHECK-NEXT: mov eax, ebx
# CHECK-NEXT: mov rax, 0x123456789ABCDEF0
# CHECK-NEXT: mov eax, 0x12345678
# CHECK-NEXT: mov rcx, [rax]
# CHECK-NEXT: mov rdx, [rax+0x10]
# CHECK-NEXT: mov r8, [rax+rbx*2+0x20]
# CHECK-NEXT: mov r9, [r12+r13*8-0x40]
# CHECK-NEXT: mov [rdi+rsi*4+0x1234], rax
# CHECK-NEXT: mov [rcx], dl
# CHECK-NEXT: mov [rcx+0x01], dx
# CHECK-NEXT: mov [rcx+0x04], edx
# CHECK-NEXT: mov [rcx+0x08], rdx
# CHECK-NEXT: add rax, rbx
# CHECK-NEXT: adc rcx, rdx
# CHECK-NEXT: sub rsi, rdi
# CHECK-NEXT: sbb r8, r9
# CHECK-NEXT: inc r10
# CHECK-NEXT: dec r11
# CHECK-NEXT: neg r12
# CHECK-NEXT: and r13, r14
# CHECK-NEXT: or r15, rax
# CHECK-NEXT: xor rbx, rbx
# CHECK-NEXT: cmp rcx, rdx
# CHECK-NEXT: test rsi, rsi
# CHECK-NEXT: add rbx, 0x7F
# CHECK-NEXT: add rcx, 0xFFFFFFFFFFFFFFFF
# CHECK-NEXT: sub rdx, 0x1234
# CHECK-NEXT: and rsi, 0xFF
# CHECK-NEXT: or rdi, 0x8000
# CHECK-NEXT: xor r8, 0x55
# CHECK-NEXT: shl rax, 0x01
# CHECK-NEXT: shr rbx, 0x01
# CHECK-NEXT: sar rcx, 0x01
# CHECK-NEXT: rol rdx, 0x01
# CHECK-NEXT: ror rsi, 0x01
# CHECK-NEXT: imul rdi, r8
# CHECK-NEXT: imul r9, r10, 0x07
# CHECK-NEXT: not r14
# CHECK-NEXT: push rax
# CHECK-NEXT: push rbx
# CHECK-NEXT: pop rcx
# CHECK-NEXT: pop rdx
# CHECK-NEXT: nop
# CHECK-NEXT: ret