#! /bin/sh

set -e

curdir=`pwd`

dev=/dev/fd0
boot=/usr/src/boot/boot
bootblock=/usr/src/boot/bootblock
image=/boot/image/3.1.2a
makeboot=/usr/src/boot

error() {
    echo "error: $1"
    exit 2
}

echo -n "The contents of /dev/fd0 will be overwritten. Are you sure? [y|N] "
read ans

test "$ans" != "y" -a "$ans" != "yes" && exit 2

if test ! -f $boot -o ! -f $bootblock ; then
    echo "]]] Making bootloader and bootblock"
    test -f $makeboot/Makefile || error "cannot find system sources"
    cd $makeboot
    make
    cd $curdir
fi

test -f $image || error "cannot find kernel image"

echo "]]] Making filesystem on $dev"
mkfs -B 1024 -b 1440 -i 512 $dev

echo "]]] Mounting $dev"
mount $dev /mnt

echo "]]] Creating directory structure"
cd /mnt
mkdir -p boot/image bin etc dev sbin

echo "]]] Installing $boot"
cp -p $boot /mnt/boot/boot
sync

echo "]]] Installing $image"
cp -p $image /mnt/boot/image/image
sync

echo "]]] Creating devices"
cd /mnt/dev
/usr/bin/MAKEDEV std
sync

echo "]]] Installing files to /bin"
for i in /bin/sh /bin/printroot /usr/bin/df ; do
    j=`basename $i`
    cp $i /mnt/bin
    sync
    echo -n "$j "
done
echo

echo "]]] Installing dummy rc file"
cat > /mnt/etc/rc << __EOF__
echo "Hello, world"
/bin/printroot > /etc/mtab
/bin/df
echo "Goodbye, cruel world"
__EOF__

echo "]]] Unmounting $dev"
cd $curdir
umount $dev

echo "]]] Installing bootblock"
installboot -d $dev $bootblock /boot/boot

echo "]]] Setting boot options"
edparams $dev 'rootdev=ram; ramimagedev=fd0; save'
edparams $dev 'leader(){echo PRESS ESC FOR PROMPT}; save'
edparams $dev 'main(){delay 2000; boot}; save'

echo "]]] Done"
