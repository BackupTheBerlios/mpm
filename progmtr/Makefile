all:

progmtr:	progmtr.o
	cc -o progmtr progmtr.o

progmtr.o:	progmtr.c
	cc -c -o progmtr.o progmtr.c

progmtr.c:	version.h

version.h:	VERSION
	@echo "#define version \"`cat VERSION`\"" > version.h

clean:
	rm -f *~ *.o progmtr
