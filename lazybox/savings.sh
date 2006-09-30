#! /bin/sh

. sources.sh

sum=0

for i in $SOURCES; do
    if test -f "/usr/bin/$i" ; then
        val=`expr -s /usr/bin/$i`
    elif test -f "/bin/$i" ; then
        val=`expr -s /bin/$i`
    else
        echo "cannot find $i" >&2
        exit 2
    fi
    sum=`expr $sum + $val`
done

lb=`expr -s lazybox`
lbp=`expr $lb \* 100 / $sum`

echo "sum of all separate binaries: $sum (100%)"
echo "size of lazybox:              $lb ($lbp%)"

