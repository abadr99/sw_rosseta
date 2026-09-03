.intel_syntax noprefix
.global _start

.text
_start:
    mov rdi, 5
    call factorial
    mov rdi, rax
    mov rax, 60
    syscall

factorial:
    mov rax, 1
.L_loop:
    cmp rdi, 1
    jle .L_done
    imul rax, rdi
    dec rdi
    jmp .L_loop
.L_done:
    ret