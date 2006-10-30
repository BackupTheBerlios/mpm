#! /bin/sh

set -e

error() {
    echo "error: $1"
    exit 2
}

curdir=`pwd`
memorysrc=/usr/src/drivers/memory/memory.c
libdriver=/usr/src/drivers/libdriver
bintocsrc=/usr/src/drivers/memory/ramdisk/bintoc.c
root=root

test -f "$memorysrc" || error "cannot find system sources ($memorysrc)"

echo "*** Building bintoc"
cc -o bintoc $bintocsrc

echo "*** Generating root.s"
./bintoc $root | sed > root.s 's/^/.data1 /;s/,$//'

echo "*** Assembling memory server"
set -x
cc -o memory -s -stack 8k -sep $memorysrc imgrd.s $libdriver/driver.o -lsys -lsysutil
set +x
