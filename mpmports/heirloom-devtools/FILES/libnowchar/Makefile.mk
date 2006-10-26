all: libnowchar.a

AR = ar
CPPFLAGS = -D_MINIX=1 -D_POSIX_SOURCE=1 -wo -ws
RANLIB = :

OBJ = nowchar.o
libnowchar.a: $(OBJ)
	$(AR) cr $@ $(OBJ)
	$(RANLIB) $@

install:

clean:
	rm -f $(OBJ) core log *~ libnowchar.a

nowchar.o: nowchar.c wchar.h
	$(CC) $(CFLAGSS) $(CPPFLAGS) $(LARGEF) -I. -c nowchar.c

MRPROPER = libwchar.a

mrproper:	clean

