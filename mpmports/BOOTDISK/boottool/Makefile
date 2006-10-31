CC       = cc
CPPFLAGS = -O

OBJ       = uprescue.o mntrescue.o bzcp.o bzlib.a boottool.o

boottool: $(OBJ)
	$(CC) -s -stack 8m -o $@ $(OBJ)

.c.o:
	$(CC) -c $(CPPFLAGS) -o $@ $<

bzlib.a:
	cd bzcp && make bzlib.a BZCPFLAGS=-DNO_MAIN && mv bzlib.a ..

bzcp.o:
	cd bzcp && make bzcp.o BZCPFLAGS=-DNO_MAIN && mv bzcp.o ..

mntrescue.o:
	cd mntrescue && make mntrescue.o MNTRESCUEFLAGS=-DNO_MAIN && mv mntrescue.o ..

uprescue.o:
	cd uprescue && make uprescue.o UPRESCUEFLAGS=-DNO_MAIN && mv uprescue.o ..

clean:
	cd bzcp && make clean
	cd uprescue && make clean
	cd mntrescue && make clean
	rm -f *~ $(OBJ) boottool
