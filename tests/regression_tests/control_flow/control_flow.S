.global _start
.text

_start:
    cmp %rbx, %rax
    jz target_label
    jnz target_label
    jl target_label
    jle target_label
    jg target_label
    jge target_label
    ja target_label
    jae target_label
    jb target_label
    jbe target_label
    jz target_label
    jnz target_label
    
    call dummy_func
    ret

target_label:
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop
    nop

dummy_func:
    push %rax
    pop %rbx
    ret
    