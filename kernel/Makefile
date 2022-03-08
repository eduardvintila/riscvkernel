DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
BOOTDIR?=$(EXEC_PREFIX)/boot
INCLUDEDIR?=$(PREFIX)/include


CFLAGS		=	-Iinclude -g -Wall -Wattributes -ffreestanding -nostdlib -fno-strict-aliasing -O2
CFLAGS		+=	-fno-omit-frame-pointer
CFLAGS		+=	-mno-save-restore -mstrict-align
CFLAGS		+=	-mabi=lp64 -march=rv64gc
CFLAGS		+=	-mcmodel=medany
CFLAGS		+=	-fno-pie -no-pie
CPPFLAGS = -D__is_kernel
LIBS = -lckernel -lgcc

OBJS  = $(patsubst %.c,%.o,$(wildcard */*.c))
OBJS += $(patsubst %.c,%.o,$(wildcard */*/*.c))
OBJS +=	$(patsubst %.S,%.o,$(wildcard */*.S))

.PHONY: all clean install install-headers install-kernel

all: test.kernel

test.kernel: $(OBJS) link.ld
	$(CC) -T link.ld -Llib $(OBJS) $(LIBS) $(CFLAGS) $(CPPFLAGS) -o $@ -Wl,--build-id=none

%.o: %.c
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.o: %.S
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

clean:
	rm -f test.kernel
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers install-kernel

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-kernel: test.kernel
	mkdir -p $(DESTDIR)$(BOOTDIR)
	cp test.kernel $(DESTDIR)$(BOOTDIR)
