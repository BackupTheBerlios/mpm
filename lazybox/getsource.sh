#! /bin/sh

dst=`basename "$1"`
base=`basename $dst .c`

cat "$1" | sed -e "s/^int main/int main_$base/" \
               -e "s/^void main/void main_$base/" \
               \
               -e 's/^void \*allocate/static void \*allocate/' \
               -e 's/^char \*whoami(/static char \*whoami(/' \
               -e 's/^char \*basename(/static char \*basename(/' \
               -e 's/^char \*getold(/static char \*getold(/' \
               -e 's/^char \*getnew(/static char \*getnew(/' \
               -e 's/^void \*xmalloc(/static void \*xmalloc(/' \
               -e 's/^void \*xrealloc(/static void \*xrealloc(/' \
               \
               -e 's/^int copy(/static int copy(/' \
               -e 's/^int find/static int find/' \
               -e 's/^int compare/static int compare/' \
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
               -e 's/^void warn/static void warn/' \
               -e 's/^void dumphunk/static void dumphunk/' \
               -e 's/^void range(/static void range(/' \
               -e 's/^void deallocate(/static void deallocate(/' \
               -e 's/^void compress(/static void compress(/' \
               -e 's/^void print(/static void print(/' \
               -e 's/^void execute(/static void execute(/' \
               -e 's/^void makedir(/static void makedir(/' \
               -e 's/^void number(/static void number(/' \
               -e 's/^void timeout(/static void timeout(/' \
               -e 's/^void path_init(/static void path_init(/' \
               -e 's/^void path_add(/static void path_add(/' \
               -e 's/^void path_trunc(/static void path_trunc(/' \
               -e 's/^mode_t parsemode(/static mode_t parsemode(/' \
               -e 's/^prettyprog/static prettyprog/' \
               \
               -e 's/^\(int force[^_]\)/static \1/' \
               -e 's/^\(int all[^_]\)/static \1/' \
               -e 's/^int plen/static int plen/' \
               -e 's/^int version/static int version/' \
               -e 's/^int column/static int column/' \
               -e 's/^int table/static int table/' \
               -e 's/^int debug/static int debug/' \
               -e 's/^int ncols/static int ncols/' \
               -e 's/^int quiet/static int quiet/' \
               -e 's/^int done/static int done/' \
               -e 's/^int xdev=/static int xdev=/' \
               -e 's/^int excode/static int excode/' \
               > $dst

