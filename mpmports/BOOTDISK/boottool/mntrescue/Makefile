CC              = cc
CPPFLAGS        = -D_MINIX=1 -O2
MNTRESCUEFLAGS ?=

OBJ      = mntrescue.o

mntrescue:	$(OBJ)
	$(CC) -s -o $@ $(OBJ)

.c.o:
	$(CC) -c $(CPPFLAGS) $(MNTRESCUEFLAGS) -o $@ $<

clean:
	rm -f *~ $(OBJ) mntrescue
