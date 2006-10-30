#! /bin/sh

set -e

mkfs=/usr/bin/mkfs
work=`pwd`/work
usr=usr
usrproto=usr.proto

echo "*** Making sure $mkfs has enough memory to work with"
chmem =`expr 128 \* 1024` $mkfs

echo "*** Deleting old $usr and/or $usr.bz2"
rm -f $usr $usr.bz2

echo "*** Creating $usr filesystem according to $usrproto"
$mkfs -B 2048 $usr $usrproto

echo "*** Compressing $usr"
bzip2 -9v $usr

echo "*** Done"
ls -l $usr.bz2
