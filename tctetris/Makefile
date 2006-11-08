
PROG     = tctetris
OBJS	 = main.o

prefix   ?= /usr/local
bindir   ?= $(prefix)/bin
dirmode  ?= 755
filemode ?= 755
owner    ?= bin
group    ?= operator

include config.mak

$(PROG):	$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) -c $(CPPFLAGS) -o $@ $<

clean:
	rm -f *~ $(OBJS) $(PROG)

distclean: clean
	rm -f config.mak

install:	$(PROG)
	install -d -m $(dirmode) -o $(owner) -g $(group) $(DESTDIR)$(bindir)
	install -m $(filemode) -o $(owner) -g $(group) $(PROG) $(DESTDIR)$(bindir)$(PROG)
