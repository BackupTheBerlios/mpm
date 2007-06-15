all:
	@echo "MPMPORTS/MINIX Bootdisk"
	@echo ""
	@echo "targets:"
	@echo ""
	@echo "fd0         create bootdisk on /dev/fd0"
	@echo "diskimage   create bootdisk.img (uses /dev/fd0)"
	@echo ""

.PHONY: fd0 diskimage

printparts/printparts:
	cd printparts && make

usr.bz2: usr.proto rc.usr printparts/printparts lazybox
	@sh mk.usr.sh

boottool/boottool:
	cd boottool && make boottool

root:	usr.bz2 root.proto boottool/boottool
	@sh mk.root.sh

memory:	root
	@sh mk.memory.sh

image: memory
	@sh mk.image.sh

fd0: image
	@sh mk.fd0.sh

diskimage: fd0
	@echo "*** Extracting bootdisk.img from /dev/fd0"
	dd if=/dev/fd0 of=bootdisk.img bs=1024 count=1440

lazybox:
	@echo "lazybox is missing"
	@echo "create lazybox manually in the lazybox directory and move it here"

clean:
	rm -f *~ bootdisk.img usr usr.bz2 root root.s bintoc memory .*~ work/* image
