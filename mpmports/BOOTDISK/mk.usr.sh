#! /bin/sh

mkfs=/usr/bin/mkfs
work=`pwd`/work
usr=usr
usrproto=usr.proto

echo "*** Making sure $mkfs has enough memory to work with"
chmem =`expr 128 \* 1024` $mkfs

echo "*** Deleting old $usr"
rm -f $usr

echo "*** Creating $usr filesystem according to $usrproto"
mkfs -B 2048 usr usr.proto
