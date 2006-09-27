PACKAGES=\
	bwk \
	cvs \
	dialog \
	expat \
	fontconfig \
	freetype \
	gnu-bash \
	gnu-coreutils \
	gnu-diffutils \
	gnu-fileutils \
	gnu-findutils \
	gnu-grep \
	gnu-gzip \
	gnu-inetutils \
	gnu-less \
	gnu-make \
	gnu-ncurses \
	gnu-sed \
	gnu-shutils \
	gnu-tar \
	gnu-textutils \
	liboop \
	lynx \
	mawk \
	minix-world \
	minix-world-source \
	pmpkg-config \
	putty \
	vim \
	xorg-lib \
	xorg-lib-pt2 \
	xorg-lib-pt3 \
	xorg-lib-pt4 \
	xorg-lib-pt5 \
	xorg-lib-pt6 \
	xorg-proto \
	xorg-util \
	xorg-xbitmaps \
	zlib \

all:
	@echo "MPM ports collection"
	@echo ""
	@echo "    make all              this message"
	@echo "    make allsrcmpm        turn all directories into .src.mpm files"
	@echo "                          (be sure you did cvs export)"
	@echo "    make clean            remove temporary files"
	@echo ""

allsrcmpm:
	@test -d CVS && echo "please use cvs export first, to avoid CVS" \
	             && echo "directories inside the packages" && exit 1
	@test -d work  || mkdir work
	@test -d SMPMS || mkdir SMPMS
	@set -e && for i in $(PACKAGES) ; do \
		echo "[[[ creating .src.mpm for $$i ]]]" ; \
		MPM_WORK=`pwd`/work MPM_SMPMS=`pwd`/SMPMS \
		/usr/mpm/bin/mpmsrc create $$i ; \
	done

clean:
	rm -f *~ */*~ */*/*~
