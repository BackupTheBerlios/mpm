
printparts:	printparts.o
	cc -s -o $@ $<

printparts.o:	printparts.c printparts.h
	cc -c -o $@ printparts.c

clean:
	rm -f *~ *.o printparts
