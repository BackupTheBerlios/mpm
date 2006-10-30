#! /bin/sh

set -e

curdir=`pwd`

dev=/dev/fd0
boot=/usr/src/boot/boot
bootblock=/usr/src/boot/bootblock
image=image
makeboot=/usr/src/boot

error() {
    echo "error: $1"
    exit 2
}

echo -n "The contents of /dev/fd0 will be overwritten. Are you sure? [y|N] "
read ans

test "$ans" != "y" -a "$ans" != "yes" && exit 2

if test ! -f $boot -o ! -f $bootblock ; then
    echo "*** Making bootloader and bootblock"
    test -f $makeboot/Makefile || error "cannot find system sources ($makeboot)"
    cd $makeboot
    make
    cd $curdir
fi

test -f $image || error "cannot find kernel image"

echo "*** Unmounting $dev"
umount $dev || true

echo "*** Making filesystem on $dev"
mkfs -B 1024 -b 1440 -i 3 $dev

echo "*** Mounting $dev"
mount $dev /mnt

echo "*** Installing $boot to /mnt/boot"
cp -p $boot /mnt/boot
sync

echo "*** Installing $image to /mnt/image"
cp -p $image /mnt/image
sync

echo "*** Unmounting $dev"
umount $dev

echo "*** Installing bootblock"
installboot -d $dev $bootblock /boot

echo "*** Setting boot options"
edparams $dev 'image=/image; save'
edparams $dev 'leader(){echo PRESS ESC FOR PROMPT}; save'
edparams $dev 'main(){delay 2000; boot}; save'

echo "*** Done"
df $dev
