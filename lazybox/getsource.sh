#! /bin/sh

dst=`basename "$1"`
base=`basename $dst .c`

cat "$1" | sed -e "s/^int main/int main_$base/" \
               -e "s/^void main/void main_$base/" \
               \
               -e 's/^void \*allocate/static void \*allocate/' \
               -e 's/^char \*whoami(/static char \*whoami(/' \
               -e 's/^char \*basename(/static char \*basename(/' \
               \
               -e 's/^int copy(/static int copy(/' \
               -e 's/^void usage(/static void usage(/' \
               -e 's/^void report(/static void report(/' \
               -e 's/^void calendar(/static void calendar(/' \
               -e 's/^void error(/static void error(/' \
               -e 's/^void regerror(/static void regerror(/' \
               -e 's/^void fatal(/static void fatal(/' \
               -e 's/^void fname(/static void fname(/' \
               -e 's/^void svcname(/static void svcname(/' \
               -e 's/^void Usage(/static void Usage(/' \
               -e 's/^void output(/static void output(/' \
               -e 's/^void onintr/static void onintr/' \
               -e 's/^void crc/static void crc/' \
               -e 's/^void addpath/static void addpath/' \
               \
               -e 's/^\(int force[^_]\)/static \1/' \
               -e 's/^int plen/static int plen/' \
               > $dst

