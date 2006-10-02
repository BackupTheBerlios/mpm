
CC         ?= cc
LD         ?= cc
CFLAGS     ?= -O -D_MINIX -I${SRCPATH} -I ${SRVSRCPATH} -I${FSSRCPATH} -w
LDFLAGS    ?= -stack 256kw -s

SOURCEPATH = /mnt/src.commands/simple
SRCPATH    = /mnt/src
SRVSRCPATH = ${SRCPATH}/servers
FSSRCPATH  = ${SRVSRCPATH}/fs

all:	lazybox

include gen.mak

lazybox:	lazybox.o ${OBJS}
	@echo ""
	@echo "LINK $@"
	@${LD} ${LDFLAGS} -o $@ lazybox.o ${OBJS}
	chmod 6755 $@

.c.o:
	@echo -n "`basename $< .c` "
	@${CC} ${CFLAGS} -c -o $@ $<

testlinks:
	@for i in ${LINKS} ; do ln -s lazybox $$i ; done

clean:
	@rm -f *.o *~ lazybox ${LINKS} gen.mak register.h elseif.h allnames.h
	@for i in ${LINKS} ; do rm -f $$i.c ; done

distclean:	clean

