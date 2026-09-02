.intel_syntax noprefix; .global _start; _start: cmp rax, rbx; jne _start; call _start; ret;
