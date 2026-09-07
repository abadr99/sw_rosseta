CC ?= gcc
CFLAGS ?= -nostdlib -static

all: factorial.elf

factorial.elf: factorial.S
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f factorial.elf

.PHONY: all clean