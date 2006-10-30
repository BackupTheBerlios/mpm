#! /bin/sh

set -e

error() {
    echo "error: $1"
    exit 2
}

curdir=`pwd`
memorysrc=/usr/src/drivers/memory/memory.c
driversrc=/usr/src/drivers/libdriver/driver.c
bintocsrc=/usr/src/drivers/memory/ramdisk/bintoc.c
root=root

test -f "$memorysrc" || error "cannot find system sources ($memorysrc)"
test -f "$driversrc" || error "cannot find system sources ($driversrc)"

echo "*** Building bintoc"
cc -o bintoc $bintocsrc

echo "*** Generating root.s"
./bintoc $root | sed > root.s 's/^/.data1 /;s/,$//'

echo "*** Assembling memory driver"
set -x
cc -o memory -s -stack 8k -sep $memorysrc imgrd.s $driversrc -lsys -lsysutil
set +x
