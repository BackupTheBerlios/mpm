
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

install:
	install -d -o ${owner} -g ${group} -m ${mode} ${DESTDIR}${bindir}
	install -d -o ${owner} -g ${group} -m ${mode} ${DESTDIR}${mandir}/man1
	install -c -o ${owner} -g ${group} -m ${mode} pmpkg-config.sh ${DESTDIR}${bindir}/pmpkg-config
	install -c -o ${owner} -g ${group} -m ${manmode} pmpkg-config.1 ${DESTDIR}${mandir}/man1/pmpkg-config.1

install-as-pkgconfig:	install
	ln -s pmpkg-config ${DESTDIR}${bindir}/pkg-config
	ln -s pmpkg-config.1 ${DESTDIR}${mandir}/man1/pkg-config.1

clean:
	rm -f *~
