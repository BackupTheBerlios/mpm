
PROG     = tctetris
OBJS	 = main.o

include config.mak

$(PROG):	$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

.c.o:
	$(CC) -c $(CPPFLAGS) -o $@ $<

clean:
	rm -f *~ $(OBJS) $(PROG)

distclean: clean
	rm -f config.mak
