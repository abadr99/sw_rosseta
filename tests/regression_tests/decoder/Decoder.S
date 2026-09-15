.global _start
.text

_start:
    mov %rbx, %rax
    mov %rdx, %rcx
    mov %r9, %r8
    mov %r11, %r10
    mov %r13, %r12
    mov %r15, %r14
    
    mov %bl, %al
    mov %bx, %ax
    mov %ebx, %eax
    
    movabs $0x123456789ABCDEF0, %rax
    mov $0x12345678, %eax
    
    mov (%rax), %rcx
    mov 0x10(%rax), %rdx
    mov 0x20(%rax,%rbx,2), %r8
    mov -0x40(%r12,%r13,8), %r9
    mov %rax, 0x1234(%rdi,%rsi,4)
    
    mov %dl, (%rcx)
    mov %dx, 0x1(%rcx)
    mov %edx, 0x4(%rcx)
    mov %rdx, 0x8(%rcx)
    
    add %rbx, %rax
    adc %rdx, %rcx
    sub %rdi, %rsi
    sbb %r9, %r8
    
    inc %r10
    dec %r11
    neg %r12
    
    and %r14, %r13
    or %rax, %r15
    xor %rbx, %rbx
    
    cmp %rdx, %rcx
    test %rsi, %rsi
    
    add $0x7F, %rbx
    add $0xFFFFFFFFFFFFFFFF, %rcx
    sub $0x1234, %rdx
    
    and $0xFF, %rsi
    or $0x8000, %rdi
    xor $0x55, %r8
    
    shl $0x01, %rax
    shr $0x01, %rbx
    sar $0x01, %rcx
    rol $0x01, %rdx
    ror $0x01, %rsi
    
    imul %r8, %rdi
    imul $0x07, %r10, %r9
    not %r14
    
    push %rax
    push %rbx
    pop %rcx
    pop %rdx
    
    nop
    ret
    