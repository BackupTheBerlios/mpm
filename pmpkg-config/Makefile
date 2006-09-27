
VERSION     =  0.90

DESTDIR     ?=
prefix      ?= /usr/local
bindir      ?= ${prefix}/bin
mandir      ?= ${prefix}/man
owner       ?= root
group       ?= root
mode        ?= 755
manmode     ?= 644
INSTALL     ?= /usr/bin/install

all:
	@echo "Poor Man's pkg-config ${VERSION}"
	@echo ""
	@echo "Targets:"
	@echo ""
	@echo "all                     print this message"
	@echo 'install                 install pmpkg-config in $${prefix}'
	@echo "install-as-pkgconfig    same as install and make pkg-config symlink"
	@echo 'uninstall               uninstall pmpkg-config from $${prefix}'
	@echo "clean                   delete ~-backup files"
	@echo ""
	@echo "Environment variables can be given on the command line:"
	@echo "    prefix=<directory>          [current: ${prefix}]"
	@echo '    bindir=$${prefix}/bin        [current: ${bindir}]'
	@echo '    mandir=$${prefix}/man        [current: ${mandir}]'
	@echo "    owner=<username>            [current: ${owner}]"
	@echo "    group=<groupname>           [current: ${group}]"
	@echo "    mode=<mode>                 [current: ${mode}]"
	@echo "    manmode=<mode>              [current: ${manmode}]"
	@echo "    INSTALL=<program>           [current: ${INSTALL}]"
	@echo ""
	@echo "Use DESTDIR=<directory> to install relative to a directory other than /"
	@echo ""

intermediate:	pmpkg-config.sh
	sed -e "s@MYPREFIX=\"/usr/local\"@MYPREFIX=\"${prefix}\"@" \
		-e "s@VERSION=0.90@VERSION=`cat VERSION`@" pmpkg-config.sh \
		> intermediate

install:	intermediate
	install -d -o ${owner} -g ${group} -m ${mode} ${DESTDIR}${bindir}
	install -d -o ${owner} -g ${group} -m ${mode} ${DESTDIR}${mandir}/man1
	install -c -o ${owner} -g ${group} -m ${mode} intermediate ${DESTDIR}${bindir}/pmpkg-config
	install -c -o ${owner} -g ${group} -m ${manmode} pmpkg-config.1 ${DESTDIR}${mandir}/man1/pmpkg-config.1

install-as-pkgconfig:	install
	ln -s pmpkg-config ${DESTDIR}${bindir}/pkg-config
	ln -s pmpkg-config.1 ${DESTDIR}${mandir}/man1/pkg-config.1

uninstall:
	rm -f ${DESTDIR}${bindir}/pmpkg-config
	rm -f ${DESTDIR}${mandir}/man1/pmpkg-config.1
	( test -L ${DESTDIR}${bindir}/pkg-config && rm -f ${DESTDIR}${bindir}/pkg-config ) || true
	( test -L ${DESTDIR}${mandir}/man1/pkg-config.1 && rm -f ${DESTDIR}${mandir}/man1/pkg-config.1 ) || true

clean:
	rm -f *~ intermediate
