CC ?= gcc
CFLAGS ?= -nostdlib -static -Wl,--build-id=none

all: Loader.elf no_exec.elf

Loader.elf: Loader.S
	$(CC) $(CFLAGS) -o $@ $<

# Builds a binary with NO executable flag on any section
no_exec.elf:
	printf '.data\n.global _start\n_start:\n.quad 1\n' | $(CC) -nostdlib -static -Wl,--build-id=none -x assembler - -o $@

clean:
	rm -f Loader.elf no_exec.elf

.PHONY: all clean