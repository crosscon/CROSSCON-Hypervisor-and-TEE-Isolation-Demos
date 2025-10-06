CC				= $(CROSS_COMPILE)gcc

CFLAGS			?= -Wall -I $(LIBFLUSH) \
				   -DDEVICE_CONFIGURATION=\"libflush/eviction/strategies/$(DEVICE_CONFIGURATION).h\" \
				   -DLLC_SIZE=$(LLC_SIZE)
ifeq ($(DEBUG), 1)
CFLAGS			+= -g -Og
endif
LDFLAGS			?= -L $(LIBFLUSH)/build/$(ARCH)/release -Wl,-Bstatic -l flush -Wl,-Bdynamic
DESTDIR			?= bin
O				?= out

ARCH			?= armv8
ARMAGEDDON		?= armageddon
LIBFLUSH		= $(ARMAGEDDON)/libflush
HAVE_PAGEMAP_ACCESS ?= 1

DEVICE_CONFIGURATION	?= rpi4
LLC_SIZE				?= 1024*1024

ifneq ($(USE_EVICTION),)
export USE_EVICTION
endif
ifneq ($(TIME_SOURCE),)
export TIME_SOURCE
endif
# end exports

BINARIES	= $(DESTDIR)/bin/cache_test

.PHONY: all $(LIBFLUSH)
all: $(BINARIES)

$(DESTDIR)/bin/%: $(LIBFLUSH) $(O)/%.o $(DESTDIR)/bin
	$(CC) $(CFLAGS) $(O)/$*.o -o $@ $(LDFLAGS) $(LDADD)

$(LIBFLUSH):
	$(MAKE) CC=$(CC) ARCH=$(ARCH) DEVICE_CONFIGURATION=$(DEVICE_CONFIGURATION) \
		HAVE_PAGEMAP_ACCESS=$(HAVE_PAGEMAP_ACCESS) -C $@

.PHONY: clean run
clean:
	rm -rf $(DESTDIR) $(O)
	$(MAKE) CC=$(CC) -C $(LIBFLUSH) clean

$(O)/%.o: %.c $(O)
	$(CC) $(CFLAGS) -c -o $@ $<

$(O):
	mkdir -p $(O)

$(DESTDIR)/bin:
	mkdir -p $(DESTDIR)/bin
