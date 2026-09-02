.intel_syntax noprefix; .global _start; _start: push rax; pop rbx; mov rcx, [rax + rbx*4 + 0x10]; add rdx, 1;
