PACKAGES=\
	bwk \
	cvs \
	dialog \
	flex \
	gnu-bash \
	gnu-coreutils \
	gnu-diffutils \
	gnu-fileutils \
	gnu-findutils \
	gnu-grep \
	gnu-gzip \
	gnu-help2man \
	gnu-inetutils \
	gnu-less \
	gnu-m4 \
	gnu-make \
	gnu-ncurses \
	gnu-sed \
	gnu-shutils \
	gnu-tar \
	gnu-textutils \
	libdockapp \
	libjpeg \
	libpng \
	libtiff \
	lynx \
	mawk \
	minix-world \
	minix-world-source \
	pdksh \
	perl \
	pmpkg-config \
	progmtr \
	putty \
	rxvt \
	xorg6 \
	vim \
	windowmaker \
	windowmaker-themes \
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

cvs-add:	clean
	test -n "${CVSADDDIR}" || exit 2
	cvs add ${CVSADDDIR} ${CVSADDDIR}/FILES ${CVSADDDIR}/PATCHES
	cvs add ${CVSADDDIR}/b* ${CVSADDDIR}/d* ${CVSADDDIR}/p*
	for i in ${CVSADDDIR}/FILES/* ; do \
		test "$$i" = "CVS" || cvs add $$i ; \
	done
	for i in ${CVSADDDIR}/PATCHES/* ; do \
		test "$$i" = "CVS" || cvs add $$i ; \
	done
	@echo "you can now run cvs ci Makefile ${CVSADDDIR}"
