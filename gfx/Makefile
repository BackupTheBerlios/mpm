PROG=gfx
OBJS=gfx.o gfx_ioctl.o vga_bios.o vga_raw.o

CPPFLAGS=-O2
LDFLAGS=-stack 256k

all:	$(PROG) test tm

$(PROG):	$(OBJS)
	cc -o $@ $(OBJS) -lsysutil -lsys $(LDFLAGS)

.c.o:
	cc -c $(CPPFLAGS) -o $@ $<

clean:
	rm -f *~ $(OBJS) $(PROG) test.o test tm

upraw:
	test -c /dev/gfx || mknod /dev/gfx c 18 0
	service up `pwd`/gfx -dev /dev/gfx -args "driver=vga_raw"

upbios:
	test -c /dev/gfx || mknod /dev/gfx c 18 0
	service up `pwd`/gfx -dev /dev/gfx -args "driver=vga_bios"

down:
	service down `cat /usr/run/gfx.pid`

test:	test.o
	cc -o test test.o

tm:	tm.o
	cc -o tm tm.o

depend:
	/usr/bin/mkdep "cc -E" *.c > .depend

include .depend
