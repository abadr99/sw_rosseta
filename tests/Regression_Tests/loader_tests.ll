# RUN: echo "This is just a plain text file, not a compiled ELF binary." > %t.fake
# RUN: not %regression_driver %t.fake 2>&1 | FileCheck %s --check-prefix=INVALID

# RUN: not %regression_driver %S/this_file_does_not_exist.elf 2>&1 | FileCheck %s --check-prefix=MISSING

# RUN: printf '\xB8\x01\x00\x00\x00\xC3' > %t.bin
# RUN: objcopy -I binary -O elf32-i386 -B i386 %t.bin %t.o
# RUN: ld -m elf_i386 -e _binary__tmp_bin_start -o %t.elf %t.o 2>/dev/null || ld -m elf_i386 -o %t.elf %t.o
# RUN: not %regression_driver %t.elf 2>&1 | FileCheck %s --check-prefix=WRONGARCH

# RUN: printf '\x7FELF' > %t.truncated
# RUN: not %regression_driver %t.truncated 2>&1 | FileCheck %s --check-prefix=TRUNCATED

# Loader error handling regression tests.
# The executable must reject malformed, missing and non-x86-64 input
# before attempting to decode .text.

# INVALID: Error: Failed to parse x86-64 ELF binary.
# MISSING: Error: Failed to parse x86-64 ELF binary.
# WRONGARCH: Error: Failed to parse x86-64 ELF binary.
# TRUNCATED: Error: Failed to parse x86-64 ELF binary.

