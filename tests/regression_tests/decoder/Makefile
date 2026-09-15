CC ?= gcc
CFLAGS ?= -nostdlib -static -Wl,--build-id=none

all: Decoder.elf

Decoder.elf: Decoder.S
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f Decoder.elf

.PHONY: all clean