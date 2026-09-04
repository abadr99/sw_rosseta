# RUN: gcc -nostdlib -Wl,--build-id=none %S/../../../tests/fibonacci.s -o %t.elf
# RUN: %rosetta %t.elf | FileCheck %s

# End-to-end regression test using the external Fibonacci x86-64
# assembly program.
#
# Assembly source:
#   tests/fibonacci.s

# CHECK: mov rcx, 0x0A
# CHECK-NEXT: mov rax, 0x00
# CHECK-NEXT: mov rbx, 0x01
# CHECK-NEXT: add rax, rbx
# CHECK-NEXT: xchg rbx, rax
# CHECK-NEXT: dec rcx
# CHECK-NEXT: jnz 0x{{[0-9a-fA-F]+}}
# CHECK-NEXT: mov rax, 0x3C
# CHECK-NEXT: xor rdi, rdi
# CHECK-NEXT: syscall
