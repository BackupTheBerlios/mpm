CC             = cc
CPPFLAGS       = -O2
UPRESCUEFLAGS ?=

OBJ      = uprescue.o

uprescue:	$(OBJ)
	$(CC) -s -o $@ $(OBJ)

.c.o:
	$(CC) -c $(CPPFLAGS) $(UPRESCUEFLAGS) -o $@ $<

clean:
	rm -f *~ $(OBJ) uprescue
