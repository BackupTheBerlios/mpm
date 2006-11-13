PROG=gfx
OBJS=gfx.o gfx_ioctl.o vga_bios.o vga_raw.o

all:	$(PROG) test tm

$(PROG):	$(OBJS)
	cc -o $@ $(OBJS) -lsysutil -lsys

.c.o:
	cc -c -o $@ $<

clean:
	rm -f *~ $(OBJS) $(PROG) test.o test

up:
	test -c /dev/gfx || mknod /dev/gfx c 18 0
	service up `pwd`/gfx -dev /dev/gfx -args "driver=vga_bios"

updebug:
	test -c /dev/gfx || mknod /dev/gfx c 18 0
	service up `pwd`/gfx -dev /dev/gfx -args "debug driver=vga_bios"

down:
	service down `cat /usr/run/gfx.pid`

test:	test.c
	cc -o test test.c

tm:	tm.c
	cc -o tm tm.c

depend:
	/usr/bin/mkdep "cc -E" *.c > .depend

include .depend
