all:
	@echo "MPMPORTS/MINIX Bootdisk"
	@echo ""
	@echo "targets:"
	@echo ""
	@echo "fd0         create bootdisk on /dev/fd0"
	@echo "diskimage   create bootdisk.img (uses /dev/fd0)"
	@echo ""

.PHONY: fd0 diskimage

fd0:
	@./mk.fd0.sh

diskimage: fd0
	@echo "*** Extracting bootdisk.img from /dev/fd0"
	dd if=/dev/fd0 of=bootdisk.img bs=1024 count=1440

clean:
	rm -f *~ bootdisk.img
