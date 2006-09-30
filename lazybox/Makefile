
CC         ?= cc
LD         ?= cc
CFLAGS     ?= -O -D_MINIX
LDFLAGS    ?= -stack 256kw -s

SOURCEPATH = /mnt/src.commands/simple

all:	lazybox

include gen.mak

lazybox:	lazybox.o ${OBJS}
	${LD} ${LDFLAGS} -o $@ lazybox.o ${OBJS}
	chmod 6755 $@

.c.o:
	${CC} ${CFLAGS} -c -o $@ $<

testlinks:
	for i in ${LINKS} ; do ln -s lazybox $$i ; done

clean:
	rm -f *.o *~ lazybox ${LINKS} gen.mak register.h elseif.h
	for i in ${LINKS} ; do rm -f $$i.c ; done

distclean:	clean

