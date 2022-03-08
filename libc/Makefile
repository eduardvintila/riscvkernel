DESTDIR?=
PREFIX?=/usr/local
EXEC_PREFIX?=$(PREFIX)
INCLUDEDIR?=$(PREFIX)/include
LIBDIR?=$(EXEC_PREFIX)/lib

CFLAGS		=	-Iinclude -g -Wall -Werror -ffreestanding -nostdlib -fno-strict-aliasing -O2
CFLAGS		+=	-fno-omit-frame-pointer
CFLAGS		+=	-mno-save-restore -mstrict-align
CFLAGS		+=	-mabi=lp64 -march=rv64gc
CFLAGS		+=	-mcmodel=medany
CPPFLAGS =
LIBC_CFLAGS:=$(CFLAGS)
LIBC_CPPFLAGS:=$(CPPFLAGS)

OBJS = $(patsubst %.c,%.o,$(wildcard */*.c))

LIBC_OBJS=$(OBJS:.o=.libc.o)

BINARIES=libckernel.a #

.PHONY: all clean install install-headers install-libs
.SUFFIXES: .o .libc.o .c .S

all: $(BINARIES)


libckernel.a: $(LIBC_OBJS)
	$(AR) rcs $@ $(LIBC_OBJS)


%.o: %.c
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.libc.o: %.c
	$(CC) -MD -c $< -o $@ $(LIBC_CFLAGS) $(LIBC_CPPFLAGS)

clean:
	rm -f $(BINARIES) *.a
	rm -f $(OBJS) $(LIBC_OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) $(LIBC_OBJS:.o=.d) *.d */*.d */*/*.d

install: install-headers install-libs

install-headers:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -R --preserve=timestamps include/. $(DESTDIR)$(INCLUDEDIR)/.

install-libs: $(BINARIES)
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp $(BINARIES) $(DESTDIR)$(LIBDIR)
