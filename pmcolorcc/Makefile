
VERSION=`cat VERSION`

prefix    += /usr/local
bindir    += ${prefix}/bin
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
	@echo "owner           owner of files/directories    [current: ${owner}]"
	@echo "group           group of files/directories    [current: ${group}]"
	@echo "mode            mode for files                [current: ${mode}]"
	@echo "dirmode         mode for directories          [current: ${dirmode}]"
	@echo "DESTDIR         install relative to...        [current: ${DESTDIR}]"
	@echo "install         installation program          [current: ${install}]"
	@echo ""

install-bindir:
	${install} -d -m ${dirmode} -o ${owner} -g ${group} ${DESTDIR}${bindir}

install-gcc:	install-bindir
	${install} -m ${mode} -o ${owner} -g ${group} pmcolorgcc ${DESTDIR}${bindir}/pmcolorgcc

install-ack:	install-bindir
	${install} -m ${mode} -o ${owner} -g ${group} pmcolorack ${DESTDIR}${bindir}/pmcolorack

install-suncc:	install-bindir
	${install} -m ${mode} -o ${owner} -g ${group} pmcolorsuncc ${DESTDIR}${bindir}/pmcolorsuncc

install-icc:	install-bindir
	${install} -m ${mode} -o ${owner} -g ${group} pmcoloricc ${DESTDIR}${bindir}/pmcoloricc

install-all:	install-gcc install-ack install-suncc install-icc

clean:
	rm -f *~
