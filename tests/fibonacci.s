.intel_syntax noprefix
.global _start

.section .text
_start:
    mov rcx, 10      # Counter: Calculate 10 iterations
    mov rax, 0       # F(0) = 0
    mov rbx, 1       # F(1) = 1

fib_loop:
    add rax, rbx     # Calculate next number
    xchg rax, rbx    # Swap rax and rbx to prepare for next loop
    dec rcx          # Decrement counter
    jnz fib_loop     # Jump back to loop if counter is not zero

    # Linux sys_exit syscall to cleanly terminate
    mov rax, 60      
    xor rdi, rdi     
    syscall