#! /bin/sh

. sources.sh

> gen.mak
> register.h
> elseif.h
> allnames.h

echo "LINKS = \\" >> gen.mak
for i in $SOURCES ; do
    echo "    $i \\" >> gen.mak
done
echo "    $CPLINKS" >> gen.mak

echo "OBJS = \\" >> gen.mak
for i in $SOURCES ; do
    echo "    $i.o \\" >> gen.mak
done
echo "" >> gen.mak

for i in $SOURCES ; do
    cat >> gen.mak <<__EOF__
$i.c:
	./getsource.sh \${SOURCEPATH}/$i.c

__EOF__
    echo "REGISTER($i);" >> register.h
    echo "ELSEIF($i);" >> elseif.h
done

for i in $CPLINKS ; do
    echo "ELSEIF2($i, cp);" >> elseif.h
done

echo "#define" ALLNAMES \"$SOURCES $CPLINKS\" >> allnames.h

