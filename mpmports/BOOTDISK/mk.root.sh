#! /bin/sh

set -e

mkfs=/usr/bin/mkfs
work=`pwd`/work
root=root
rootproto=root.proto

echo "*** Making sure $mkfs has enough memory to work with"
chmem =`expr 128 \* 1024` $mkfs

echo "*** Deleting old $root and/or $root.bz2"
rm -f $root $root.bz2

echo "*** Creating $root filesystem according to $rootproto"
$mkfs -B 2048 $root $rootproto

echo "*** Done"
ls -l $root
