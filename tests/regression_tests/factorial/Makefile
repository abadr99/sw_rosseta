	CC ?= gcc
	CFLAGS ?= -nostdlib -static -Wl,--build-id=none

	all: Factorial.elf

	Factorial.elf: Factorial.S
		$(CC) $(CFLAGS) -o $@ $<

	clean:
		rm -f Factorial.elf

	.PHONY: all clean
