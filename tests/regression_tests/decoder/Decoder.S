.intel_syntax noprefix
.global _start
.text

_start:
    mov rax, rbx
    mov rcx, rdx
    mov r8, r9
    mov r10, r11
    mov r12, r13
    mov r14, r15
    mov al, bl
    mov ax, bx
    mov eax, ebx
    mov rax, 0x123456789abcdef0
    mov eax, 0x12345678
    mov rcx, [rax]
    mov rdx, [rax + 0x10]
    mov r8, [rax + rbx*2 + 0x20]
    mov r9, [r12 + r13*8 - 0x40]
    mov [rdi + rsi*4 + 0x1234], rax
    mov byte ptr [rcx], dl
    mov word ptr [rcx+1], dx
    mov dword ptr [rcx+4], edx
    mov qword ptr [rcx+8], rdx
    add rax, rbx
    adc rcx, rdx
    sub rsi, rdi
    sbb r8, r9
    inc r10
    dec r11
    neg r12
    and r13, r14
    or r15, rax
    xor rbx, rbx
    cmp rcx, rdx
    test rsi, rsi
    add rbx, 0x7f
    add rcx, -1
    sub rdx, 0x1234
    and rsi, 0xff
    or rdi, 0x8000
    xor r8, 0x55
    shl rax, 1
    shr rbx, 1
    sar rcx, 1
    rol rdx, 1
    ror rsi, 1
    imul rdi, r8
    imul r9, r10, 7
    not r14
    push rax
    push rbx
    pop rcx
    pop rdx
    nop
    ret