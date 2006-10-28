PACKAGES=`cat STATUS/up-to-date`

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
	@test -d WORK  || mkdir WORK
	@test -d SMPMS || mkdir SMPMS
	@set -e && for i in $(PACKAGES) ; do \
		echo "[[[ creating .src.mpm for $$i ]]]" ; \
		MPM_WORK=`pwd`/WORK MPM_SMPMS=`pwd`/SMPMS \
		/usr/mpm/bin/mpmsrc create $$i ; \
	done

clean:
	rm -f *~ */*~ */*/*~

# does NOT work if FILES/ or PATCHES/ have subdirectories!
cvs-add:	clean
	test -n "${CVSADDDIR}" || exit 2
	cvs add ${CVSADDDIR} ${CVSADDDIR}/FILES ${CVSADDDIR}/PATCHES
	cvs add ${CVSADDDIR}/b* ${CVSADDDIR}/d* ${CVSADDDIR}/p*
	for i in ${CVSADDDIR}/FILES/* ${CVSADDDIR}/PATCHES/* ; do \
		j=`echo "$$i" | grep "CVS"` ; \
		test -n "$$j" || cvs add $$i ; \
	done
	@echo "Files and directories added. Use cvs ci to commit."
