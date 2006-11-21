include config.mak

PROG=gfx
OBJS=gfx.o gfx_ioctl.o fonts.o generic.o $(DRIVEROBJS)

#CPPFLAGS=-O2
LDFLAGS=-stack 256k -s

all:	$(PROG) test tm

$(PROG):	$(OBJS)
	cc -o $@ $(OBJS) -lsysutil -lsys $(LDFLAGS)

.c.o:
	cc -c $(CPPFLAGS) -o $@ $<

clean:
	rm -f .*~ *~ $(OBJS) $(PROG) test.o test tm tm.o config.h config.mak .depend

/dev/gfx:
	mknod /dev/gfx c 18 0

upvesa:	$(PROG) /dev/gfx test tm
	service up `pwd`/gfx -dev /dev/gfx -args "debug driver=svga_vesa"

upraw:	$(PROG) /dev/gfx test tm
	service up `pwd`/gfx -dev /dev/gfx -args "driver=vga_raw"

upbios:	$(PROG) /dev/gfx test tm
	service up `pwd`/gfx -dev /dev/gfx -args "driver=vga_bios"

down:
	service down `cat /usr/run/gfx.pid`

test:	test.o
	cc -o test test.o -s

tm:	tm.o
	cc -o tm tm.o -s

depend:
	/usr/bin/mkdep "cc -E" *.c > .depend

include .depend
