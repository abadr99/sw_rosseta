.global _start
.intel_syntax noprefix
.text

_start:
    mov rdi, 5
    call factorial
    mov rax, 60
    syscall

factorial:
    mov rax, 1
.loop:
    cmp rdi, 1
    jle .done
    imul rax, rdi
    dec rdi
    jmp .loop
.done:
    ret