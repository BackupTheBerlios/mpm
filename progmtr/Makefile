VERSION = `cat VERSION`

DISTDIR   = progmtr-${VERSION}
DISTFILES = progmtr.c progmtr.1 LICENSE VERSION Makefile AUTHORS README

CC       ?= cc
LD       ?= cc -stack 24k -s
CFLAGS   ?= -O
LDFLAGS  ?=

DESTDIR  ?=
prefix   ?= /usr/local
bindir   ?= ${prefix}/bin
mandir   ?= ${prefix}/man
install  ?= /usr/bin/install
owner    ?= bin
group    ?= operator
dirmode  ?= 755
execmode ?= 755
manmode  ?= 644

all:
	@echo "progmtr - progress meter ${VERSION} - Copyright (c) 2006, Ivo van Poorten"
	@echo ""
	@echo "targets:"
	@echo ""
	@echo "all             print this message"
	@echo "progmtr         build progmtr executable"
	@echo "install         build and install progmtr executable and manual page"
	@echo "install-exec    build and install progmtr executable"
	@echo "install-data    install progmtr manual page"
	@echo "clean           remove ~-files, objects, executables and generated files"
	@echo ""
	@echo "enironment:"
	@echo ""
	@echo 'DESTDIR     install relative to $${DESTDIR}  [current: ${DESTDIR}]'
	@echo "prefix      installation prefix             [current: ${prefix}]"
	@echo "bindir      install executables here        [current: ${bindir}]"
	@echo "mandir      install manual pages here       [current: ${mandir}]"
	@echo "install     installation program            [current: ${install}]"
	@echo "owner       owner of files and directories  [current: ${owner}]"
	@echo "group       group of files and directories  [current: ${group}]"
	@echo "dirmode     mode for directories            [current: ${dirmode}]"
	@echo "execmode    mode for executables            [current: ${execmode}]"
	@echo "manmode     mode for manual pages           [current: ${manmode}]"
	@echo "CC          c compiler                      [current: ${CC}]"
	@echo "LD          linker                          [current: ${LD}]"
	@echo "CFLAGS      compiler flags                  [current: ${CFLAGS}]"
	@echo "LDFLAGS     linker flags                    [current: ${LDFLAGS}]"
	@echo ""

progmtr:	progmtr.o
	${LD} ${LDFLAGS} -o progmtr progmtr.o

progmtr.o:	progmtr.c version.h
	${CC} ${CFLAGS} -c -o progmtr.o progmtr.c

version.h:	VERSION
	echo "#define version \"`cat VERSION`\"" > version.h

install-exec:	progmtr
	${install} -d -o ${owner} -g ${group} -m ${dirmode} ${DESTDIR}${bindir}
	${install} -c -o ${owner} -g ${group} -m ${execmode} progmtr ${DESTDIR}${bindir}/progmtr

install-data:
	${install} -d -o ${owner} -g ${group} -m ${dirmode} ${DESTDIR}${mandir}/man1
	${install} -c -o ${owner} -g ${group} -m ${manmode} progmtr.1 ${DESTDIR}${mandir}/man1/progmtr.1

install:	install-exec install-data

clean:
	rm -f *~ *.o progmtr version.h

distclean:	clean
	rm -f ${DISTDIR}.tar.bz2

dist:	distclean
	mkdir ${DISTDIR}
	cp ${DISTFILES} ${DISTDIR}
	tar cf ${DISTDIR}.tar ${DISTDIR}
	bzip2 -9 ${DISTDIR}.tar
	rm -rf ${DISTDIR}

