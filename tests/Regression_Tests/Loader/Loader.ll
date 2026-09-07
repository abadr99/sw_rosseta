; RUN: make -C %S all

; 1. Valid ELF containing an executable section
; RUN: %rosetta --input %S/Loader.elf --output /dev/null --stop-after loader > %t
; RUN: FileCheck %s --check-prefix=CHECK-SUCCESS < %t

; CHECK-SUCCESS: [Loader] Successfully loaded executable section

; 2. Binary with no executable sections (GetExecutableCode returns empty Data)
; RUN: not %rosetta --input %S/no_exec.elf --output /dev/null --stop-after loader 2>&1 | FileCheck %s --check-prefix=CHECK-NOEXEC

; CHECK-NOEXEC: Error: No executable section found.

; 3. Non-ELF / Plain Text file
; RUN: echo "Not an ELF file" > %t.fake
; RUN: not %rosetta --input %t.fake --output /dev/null --stop-after loader 2>&1 | FileCheck %s --check-prefix=CHECK-INVALID

; CHECK-INVALID: Error: Failed to parse x86-64 ELF binary.

; 4. Non-Existent File
; RUN: not %rosetta --input %S/non_existent.elf --output /dev/null --stop-after loader 2>&1 | FileCheck %s --check-prefix=CHECK-MISSING

; CHECK-MISSING: Error: Failed to parse x86-64 ELF binary.

; 5. Truncated ELF header
; RUN: printf '\x7FELF' > %t.truncated
; RUN: not %rosetta --input %t.truncated --output /dev/null --stop-after loader 2>&1 | FileCheck %s --check-prefix=CHECK-TRUNCATED

; CHECK-TRUNCATED: Error: Failed to parse x86-64 ELF binary.