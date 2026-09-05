# RUN: printf '%%s\n' \
# RUN: '.intel_syntax noprefix' \
# RUN: '.global _start' \
# RUN: '_start:' \
# RUN: '  cmp rax, rbx' \
# RUN: '  je equal' \
# RUN: '  jne not_equal' \
# RUN: '  jl less' \
# RUN: '  jle less_equal' \
# RUN: '  jg greater' \
# RUN: '  jge greater_equal' \
# RUN: '  ja above' \
# RUN: '  jae above_equal' \
# RUN: '  jb below' \
# RUN: '  jbe below_equal' \
# RUN: '  jz zero' \
# RUN: '  jnz nonzero' \
# RUN: '  call target' \
# RUN: '  ret' \
# RUN: 'equal: nop' \
# RUN: 'not_equal: nop' \
# RUN: 'less: nop' \
# RUN: 'less_equal: nop' \
# RUN: 'greater: nop' \
# RUN: 'greater_equal: nop' \
# RUN: 'above: nop' \
# RUN: 'above_equal: nop' \
# RUN: 'below: nop' \
# RUN: 'below_equal: nop' \
# RUN: 'zero: nop' \
# RUN: 'nonzero: nop' \
# RUN: 'target:' \
# RUN: '  push rax' \
# RUN: '  pop rbx' \
# RUN: '  ret' \
# RUN: > %t.s
# RUN: gcc -nostdlib -Wl,--build-id=none %t.s -o %t.elf
# RUN: %regression_driver %t.elf | FileCheck %s

# Control-flow regression.

# CHECK: cmp rax, rbx
# CHECK-NEXT: jz 0x{{[0-9a-fA-F]+}}
# CHECK-NEXT: jnz 0x{{[0-9a-fA-F]+}}
# CHECK-NEXT: jl 0x{{[0-9a-fA-F]+}}
# CHECK-NEXT: jle 0x{{[0-9a-fA-F]+}}
# CHECK-NEXT: jnle 0x{{[0-9a-fA-F]+}}
# CHECK-NEXT: jnl 0x{{[0-9a-fA-F]+}}
# CHECK-NEXT: jnbe 0x{{[0-9a-fA-F]+}}
# CHECK-NEXT: jnb 0x{{[0-9a-fA-F]+}}
# CHECK-NEXT: jb 0x{{[0-9a-fA-F]+}}
# CHECK-NEXT: jbe 0x{{[0-9a-fA-F]+}}
# CHECK-NEXT: jz 0x{{[0-9a-fA-F]+}}
# CHECK-NEXT: jnz 0x{{[0-9a-fA-F]+}}
# CHECK-NEXT: call 0x{{[0-9a-fA-F]+}}
# CHECK-NEXT: ret
# CHECK: nop
# CHECK: push rax
# CHECK-NEXT: pop rbx
# CHECK-NEXT: ret

