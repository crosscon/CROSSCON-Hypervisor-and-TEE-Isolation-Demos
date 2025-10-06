CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
CFILES = $(wildcard *.c)
OFILES = $(CFILES:.c=.o)
GCCFLAGS = -Wall -Werror -O2 -ffreestanding -nostdlib -nostartfiles -mstrict-align
CONFIG_TEXT_BASE ?= 0x20200000

all: clean kernel8.img

boot.o: boot.S
	$(CC) $(GCCFLAGS) -c boot.S -o boot.o

%.o: %.c
	$(CC) $(GCCFLAGS) -c $< -o $@

kernel8.img: boot.o $(OFILES)
	$(CC) -E -P -x c -DCONFIG_TEXT_BASE=$(CONFIG_TEXT_BASE) link.ld.in >link.ld
	$(LD) -nostdlib boot.o $(OFILES) -T link.ld -o kernel8.elf
	$(OBJCOPY) -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.elf *.o *.img link.ld >/dev/null 2>&1 || true
