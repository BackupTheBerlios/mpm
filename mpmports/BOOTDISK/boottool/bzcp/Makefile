CC         = cc
# compile with -DNO_CRC to save another 1136 bytes, but beware: decompression
# errors are NOT detected anymore!
CPPFLAGS   = -D_POSIX_SOURCE=1
BZCPFLAGS ?=

OBJ      = bzcp.o
LIBOBJ   = blocksort.o bzlib.o crctable.o decompress.o huffman.o randtable.o

LIB=bzlib.a

bzcp:	$(LIB) $(OBJ)
	$(CC) -s -stack 8m -o $@ $(OBJ) $(LIB)

$(LIB):	$(LIBOBJ)
	ar cr $@ $(LIBOBJ)

.c.o:
	$(CC) -c $(CPPFLAGS) $(BZCPFLAGS) -o $@ $<

clean:
	rm -f *~ $(LIBOBJ) $(LIB) $(OBJ) bzcp
