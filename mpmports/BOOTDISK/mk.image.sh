#! /bin/sh

set -e

curdir=`pwd`
vanilla=/boot/image/3.1.2a
work=$curdir/work

error() {
    echo "error: $1"
    exit 2
}

test -f "$vanilla" || error "cannot find vanilla kernel ($vanilla)"
test -d "$work" || mkdir $work

echo "*** Disassembling $vanilla"
cd $work
installboot -x $vanilla

echo "*** Assembling new image"
installboot -i $curdir/image kernel pm fs rs ds tty $curdir/memory log init

echo "*** Done"
