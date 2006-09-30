#! /bin/sh

dst=`basename "$1"`
base=`basename $dst .c`

cat "$1" | sed -e "s/int main/int main_$base/" \
               -e "s/void main/void main_$base/" \
               -e 's/^void \*allocate/static void \*allocate/' \
               -e 's/^int copy(/static int copy(/' \
               -e 's/^void usage(/static void usage(/' \
               -e 's/^void report(/static void report(/' \
               > $dst

