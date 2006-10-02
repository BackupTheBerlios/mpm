#! /bin/sh

. sources.sh

> gen.mak
> register.h
> elseif.h
> allnames.h

echo -n "gen.mak: "

echo "LINKS = \\" >> gen.mak
for i in $SOURCES ; do
    echo "    $i \\" >> gen.mak
done
echo "    $CPLINKS" >> gen.mak

echo -n "(sources) "

echo "OBJS = \\" >> gen.mak
for i in $SOURCES ; do
    echo "    $i.o \\" >> gen.mak
done
echo "" >> gen.mak

echo -n "(objects) "

for i in $SOURCES ; do
    cat >> gen.mak <<__EOF__
$i.c:
	@./getsource.sh \${SOURCEPATH}/$i.c

__EOF__
done

echo -n "(dependencies) "
echo "ok "

echo -n "header files: "

for i in $SOURCES ; do
    j=`echo $i | tr '.' '_'`
    if test "$i" = "$j" ; then
        echo "REGISTER($i);" >> register.h
        echo "ELSEIF($i);" >> elseif.h
    else
        echo "REGISTER($j);" >> register.h
        echo "ELSEIF2($i, $j);" >> elseif.h
    fi
done

for i in $CPLINKS ; do
    echo "ELSEIF2($i, cp);" >> elseif.h
done

echo -n "register.h elseif.h "

echo "#define" ALLNAMES \"$SOURCES $CPLINKS\" >> allnames.h

echo -n "allnames.h "

echo ""

