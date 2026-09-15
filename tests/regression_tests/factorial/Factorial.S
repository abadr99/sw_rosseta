// Factorial.S
//
// [BUILD]: gcc -static -Wl,--build-id=none -o Factorial.elf Factorial.S
// [  RUN]: ./Factorial.elf
// [CHECK]: echo $?

.text
.global main

main:
    movq    $4, %rcx
    movq    $1, %rax

    cmpq    $1, %rcx
    jle     .Lfactorial_end

.Lfactorial_loop:
    imulq   %rcx, %rax
    decq    %rcx
    cmpq    $1, %rcx
    jg      .Lfactorial_loop

.Lfactorial_end:
    ret
