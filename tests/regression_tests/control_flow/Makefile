CC ?= gcc
CFLAGS ?= -nostdlib -static -Wl,--build-id=none

all: control_flow.elf

control_flow.elf: control_flow.S
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f control_flow.elf

.PHONY: all clean