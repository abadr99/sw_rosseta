; RUN: make -C %S all
; RUN: %rosetta --input %S/factorial.elf --output /dev/null --stop-after decoder --dump-input-instructions=true > %t
; RUN: FileCheck %s < %t

; CHECK: mov rdi, 0x05
; CHECK: call
; CHECK: mov rax, 0x01
; CHECK: cmp rdi, 0x01
; CHECK: jle
; CHECK: imul rax, rdi
; CHECK: dec rdi
; CHECK: jmp
; CHECK: ret