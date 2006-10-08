
VERSION    = `cat VERSION`
DISTDIR    = pmcolorcc-${VERSION}

DISTFILES  = AUTHORS LICENSE Makefile README TODO VERSION pmcolorack \
             pmcolorackrc pmcolorgcc pmcolorgccrc pmcoloricc pmcoloriccrc \
             pmcolorsuncc pmcolorsunccrc

prefix    += /usr/local
bindir    += ${prefix}/bin
etcdir    += ${prefix}/etc
owner     += root
group     += root
mode      += 755
dirmode   += 755
DESTDIR   +=
install   += /usr/bin/install

all:
	@echo "Poor Man's colorcc ${VERSION} - copyright (c) 2006, Ivo van Poorten"
	@echo ""
	@echo "targets:"
	@echo ""
	@echo "all             print this message"
	@echo "install-gcc     install pmcolorgcc"
	@echo "install-ack     install pmcolorack"
	@echo "install-suncc   install pmcolorsuncc"
	@echo "install-icc     install pmcoloricc"
	@echo "install-all     install all of the above"
	@echo ""
	@echo "environment:"
	@echo ""
	@echo "prefix          installation prefix           [current: ${prefix}]"
	@echo "bindir          install binaries here         [current: ${bindir}]"
	@echo "etcdir          install sample rc files here  [current: ${etcdir}]"
	@echo "owner           owner of files/directories    [current: ${owner}]"
	@echo "group           group of files/directories    [current: ${group}]"
	@echo "mode            mode for files                [current: ${mode}]"
	@echo "dirmode         mode for directories          [current: ${dirmode}]"
	@echo "DESTDIR         install relative to...        [current: ${DESTDIR}]"
	@echo "install         installation program          [current: ${install}]"
	@echo ""

install-bindir:
	${install} -d -m ${dirmode} -o ${owner} -g ${group} ${DESTDIR}${bindir}

install-etcdir:
	${install} -d -m ${dirmode} -o ${owner} -g ${group} ${DESTDIR}${etcdir}

install-gcc:	install-bindir install-etcdir
	${install} -m ${mode} -o ${owner} -g ${group} pmcolorgcc ${DESTDIR}${bindir}/pmcolorgcc
	${install} -m ${mode} -o ${owner} -g ${group} pmcolorgccrc ${DESTDIR}${etcdir}/pmcolorgccrc

install-ack:	install-bindir install-etcdir
	${install} -m ${mode} -o ${owner} -g ${group} pmcolorack ${DESTDIR}${bindir}/pmcolorack
	${install} -m ${mode} -o ${owner} -g ${group} pmcolorackrc ${DESTDIR}${etcdir}/pmcolorackrc

install-suncc:	install-bindir install-etcdir
	${install} -m ${mode} -o ${owner} -g ${group} pmcolorsuncc ${DESTDIR}${bindir}/pmcolorsuncc
	${install} -m ${mode} -o ${owner} -g ${group} pmcolorsunccrc ${DESTDIR}${etcdir}/pmcolorsunccrc

install-icc:	install-bindir install-etcdir
	${install} -m ${mode} -o ${owner} -g ${group} pmcoloricc ${DESTDIR}${bindir}/pmcoloricc
	${install} -m ${mode} -o ${owner} -g ${group} pmcoloriccrc ${DESTDIR}${etcdir}/pmcoloriccrc

install-all:	install-gcc install-ack install-suncc install-icc

clean:
	rm -f *~

distclean:	clean
	rm -rf ${DISTDIR}.tar.bz2 ${DISTDIR}.tar ${DISTDIR}

dist:	distclean
	mkdir ${DISTDIR}
	cp ${DISTFILES} ${DISTDIR}
	tar cvf ${DISTDIR}.tar ${DISTDIR}
	bzip2 -9 ${DISTDIR}.tar
	rm -rf ${DISTDIR}

