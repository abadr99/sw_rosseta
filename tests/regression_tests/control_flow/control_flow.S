.intel_syntax noprefix
.global _start
.text

_start:
    cmp rax, rbx
    je equal
    jne not_equal
    jl less
    jle less_equal
    jg greater
    jge greater_equal
    ja above
    jae above_equal
    jb below
    jbe below_equal
    jz zero
    jnz nonzero
    call target
    ret

equal: nop
not_equal: nop
less: nop
less_equal: nop
greater: nop
greater_equal: nop
above: nop
above_equal: nop
below: nop
below_equal: nop
zero: nop
nonzero: nop

target:
    push rax
    pop rbx
    ret