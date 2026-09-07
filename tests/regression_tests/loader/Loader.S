.intel_syntax noprefix
.global _start
.text

_start:
    mov rax, 60
    xor rdi, rdi
    syscall