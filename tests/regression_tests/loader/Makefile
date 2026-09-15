CC ?= gcc
CFLAGS ?= -nostdlib -static -Wl,--build-id=none

all: Loader.elf

Loader.elf: Loader.S
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f Loader.elf

.PHONY: all clean