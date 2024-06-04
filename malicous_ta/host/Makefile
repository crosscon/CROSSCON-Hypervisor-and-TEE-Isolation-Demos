CC      ?= $(CROSS_COMPILE)gcc
LD      ?= $(CROSS_COMPILE)ld
AR      ?= $(CROSS_COMPILE)ar
NM      ?= $(CROSS_COMPILE)nm
OBJCOPY ?= $(CROSS_COMPILE)objcopy
OBJDUMP ?= $(CROSS_COMPILE)objdump
READELF ?= $(CROSS_COMPILE)readelf

OBJS += main.o

CFLAGS += -Wall -I../ta/include -I./include -I$(TEEC_EXPORT)/include -static
#Add/link other required libraries here
LDFLAGS += -L$(TEEC_EXPORT)/lib -lteec -lpthread

BINARY = malicious_ca

.PHONY: all
all: $(BINARY)

$(BINARY): $(OBJS)
	$(CC) $(CFLAGS) main.c -o $(BINARY) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(BINARY)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

