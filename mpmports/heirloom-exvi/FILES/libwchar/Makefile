all: libwchar.a

AR = ar
CPPFLAGS = -D_MINIX=1 -D_POSIX_SOURCE=1 -wo -ws
RANLIB = :

OBJ = mbtowc.o wctomb.o wctype.o mbstowcs.o wcwidth.o wcslen.o wcsncmp.o \
	mbrtowc.o wctfunc.o mblen.o
libwchar.a: $(OBJ)
	$(AR) cr $@ $(OBJ)
	$(RANLIB) $@

install:

clean:
	rm -f $(OBJ) core log *~ libwchar.a

mbtowc.o: mbtowc.c wchar.h
	$(CC) $(CFLAGSS) $(CPPFLAGS) $(LARGEF) -I. -c mbtowc.c

mblen.o: mblen.c wchar.h
	$(CC) $(CFLAGSS) $(CPPFLAGS) $(LARGEF) -I. -c mblen.c

mbrtowc.o: mbrtowc.c wchar.h
	$(CC) $(CFLAGSS) $(CPPFLAGS) $(LARGEF) $(ICOMMON) -I. -c mbrtowc.c

wctomb.o: wctomb.c wchar.h
	$(CC) $(CFLAGSS) $(CPPFLAGS) $(LARGEF) $(ICOMMON) -I. -c wctomb.c

mbstowcs.o: mbstowcs.c wchar.h
	$(CC) $(CFLAGSS) $(CPPFLAGS) $(LARGEF) $(ICOMMON) -I. -c mbstowcs.c

wcwidth.o: wcwidth.c wchar.h
	$(CC) $(CFLAGSS) $(CPPFLAGS) $(LARGEF) $(ICOMMON) -I. -c wcwidth.c

wcslen.o: wcslen.c wchar.h
	$(CC) $(CFLAGSS) $(CPPFLAGS) $(LARGEF) $(ICOMMON) -I. -c wcslen.c

wcsncmp.o: wcsncmp.c wchar.h
	$(CC) $(CFLAGSS) $(CPPFLAGS) $(LARGEF) $(ICOMMON) -I. -c wcsncmp.c

wctype.o: wctype.c wchar.h
	$(CC) $(CFLAGSS) $(CPPFLAGS) $(LARGEF) $(ICOMMON) -I. -c wctype.c

wctfunc.o: wctfunc.c wchar.h
	$(CC) $(CFLAGSS) $(CPPFLAGS) $(LARGEF) $(ICOMMON) -I. -c wctfunc.c

MRPROPER = libwchar.a
