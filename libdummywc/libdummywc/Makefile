DESTDIR ?=
prefix  ?= /usr/local
libdir  ?= ${prefix}/lib
owner   ?= bin
group   ?= operator
dirmode ?= 755
mode    ?= 644

libdummywc.a:	libdummywc.o
	ar cr libdummywc.a libdummywc.o

libdummywc.o:	libdummywc.c
	cc -c -D_MINIX=1 -D_POSIX_SOURCE=1 -o libdummywc.o libdummywc.c

clean:
	rm -f *~ *.a *.o

install:	libdummywc.a
	install -d -o ${owner} -g ${group} -m ${dirmode} ${DESTDIR}${libdir}
	install -c -o ${owner} -g ${group} -m ${mode} libdummywc.a ${DESTDIR}${libdir}/libdummywc.a

