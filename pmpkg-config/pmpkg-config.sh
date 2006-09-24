#! /bin/sh

#
# Poor Man's Package Config
#
# Copyright (c) 2006 by Ivo van Poorten
# All rights reserved.
#

VERSION=0.90

test "$#" = "0" && echo "Must specify package names on the command line" &&
    exit 2

case " $@" in
    \ --help*)
    echo "Poor Man's pkg-config $VERSION"
    echo "Usage: $0 [OPTION...]"
    echo "  --version                               output version of $0"
    echo "  --modversion                            output version for package"
    echo "  --atleast-pkgconfig-version=VERSION     require given version of"
    echo "                                          pkg-config (always true)"
    echo "  --libs                                  output all linker flags"
    echo "  --static                                output linker flags for"
    echo "                                          static linking"
    exit 2
esac

case "$@" in
    *--version*)      echo "$VERSION" ; exit 0 ;;
    *--atleast-pkgconfig-version*)      exit 0 ;;
esac

_plus_private=no

_exists=no
_cflagsI=no
_cflagso=no
_libsl=no
_libsL=no
_libso=no
_modversion=no

_CFLAGS=""
_LIBSl=""
_LIBSL=""
_MODVERSIONS=""

PKG_CONFIG_PATH=${PKG_CONFIG_PATH-" "}

nmod=0

parse_modname() {
    eval _mod_$nmod=$1
    if test -n "$2" ; then
        case "$2" in
            \<\=)   c=lteq  ;;
            \<)     c=lt    ;;
            \=)     c=eq    ;;
            \!\=)   c=neq   ;;
            \>)     c=gt    ;;
            \>\=)   c=gteq  ;;
        esac
        eval _mod_${nmod}_c=$c
        eval _mod_${nmod}_v=$3
    else
        eval _mod_${nmod}_c=ok
    fi

    nmod=`expr $nmod + 1`
}

parse_cmd_line() {
    while test "$#" != "0" ; do
        case "$1" in
            --exists)               _exists=yes ;;
            --cflags)               _cflagsI=yes _cflagso=yes ;;
            --cflags-only-I)        _cflagsI=yes _cflagso=no ;;
            --cflags-only-other)    _cflagsI=no  _cflagso=yes ;;
            --libs)                 _libsl=yes _libsL=yes _libso=yes  ;;
            --libs-only-l)          _libsl=yes _libsL=no  _libso=no   ;;
            --libs-only-L)          _libsl=no  _libsL=yes _libso=no  ;;
            --libs-only-other)      _libsl=no  _libsL=no  _libso=yes ;;
            --static)               _plus_private=yes ;;
            --modversion)           _modversion=yes ;;
            --*)            ;;  # silently ignore unknown options
            -*)             ;;
            [abcdefghijklmnopqrstuvwxyz0123456789]*)
                case "$2" in
                    \<*|\=*|\>*|\!*)
                        parse_modname $1 $2 $3
                        shift 2
                        ;;
                    *)
                        parse_modname $1
                esac
                ;;
            *)  echo "error parsing command line: $1"; exit 2 ;;
        esac
        shift 1
    done
}

find_file() {
    for i in /usr/lib/pkgconfig /usr/local/lib/pkgconfig `IFS=":"; echo $PKG_CONFIG_PATH` ; do
        test -f $i/$1.pc && echo $i/$1.pc && return
    done
    echo "No package '$1' found" >&2
    echo "notfound"
}

# echo the list. do not echo an entry if it also occurs later in the list
remove_doubles() {
    while test "$#" != "0"; do
        me=$1
        shift 1
        case " $@ " in
            *\ $me\ *)    ;;
            *)          echo "$me" ;;
        esac
    done
}

recurse_requires() {
    test "$#" = "0" && return
    while test "$#" != "0" ; do
        echo $1
        file=`find_file $1`
        get_requires $1 $file
        shift 1
    done
}

get_requires() {
    if test "$2" = "notfound" ; then echo "notfound"; exit ; fi
    if test "$_plus_private" = "yes" ; then
        t="`grep '^Requires:' $2 | cut -d ':' -f 2` `cat $2 | grep '^Requires.private:' | cut -d ':' -f 2`"
        t=`remove_doubles $t`
        recurse_requires $t
    else
        recurse_requires `grep '^Requires:' $2 | cut -d ':' -f 2`
    fi
}

all_requires() {
    n=0
    while test "$n" != "$nmod" ; do
        mod=`eval echo \\$_mod_$n`
        recurse_requires $mod
        n=`expr $n + 1`
    done
}

data_from_file() {
    field=$1
    file=$2
    env=`cat $file | sed -e '/^[ ]*$/q'`
    flags=`cat $file | grep "^$field:" | cut -d ':' -f 2`
    output=`eval "eval $env"; eval echo $flags`
    echo $output
}

collect_data() {
    field=$1
    shift 1
    while test "$#" != "0" ; do
        file=`find_file $1`
        data_from_file $field $file
        shift 1
    done
}

extract() {
    which=$1
    shift 1
    while test "$#" != "0" ; do
        case $which in
            L)  case "$1" in
                    \-L*|\-R*)      echo $1 ;;
                esac
                ;;
            l)  case "$1" in
                    \-l*)           echo $1 ;;
                esac
                ;;
            lo) case "$1" in
                    \-l*|\-L*|\-R*)         ;;
                    *)              echo $1 ;;
                esac
                ;;
            D)  case "$1" in
                    \-I*)   ;;
                    *)      echo $1 ;;
                esac
                ;;
            I)  case "$1" in
                    \-I*)   echo $1 ;;
                esac
                ;;
        esac
        shift 1
    done
}

printop() {
    case "$1" in
        lteq)   echo "<="   ;;
        lt)     echo "<"    ;;
        eq)     echo "="    ;;
        neq)    echo "!="   ;;
        gt)     echo ">"    ;;
        gteq)   echo ">="   ;;
    esac
}

# number filename
check_constraints() {
    mod=`eval echo \\$_mod_$1`
    c=`eval echo \\$_mod_$1_c`
    v=`eval echo \\$_mod_$1_v`

    test "$c" = "ok" && exit 0

    maj=`echo $v.0.0.0 | cut -d '.' -f 1`
    min=`echo $v.0.0.0 | cut -d '.' -f 2`
    sub=`echo $v.0.0.0 | cut -d '.' -f 3`

    modver=`data_from_file Version $2`
    modmaj=`echo $modver.0.0.0 | cut -d '.' -f 1`
    modmin=`echo $modver.0.0.0 | cut -d '.' -f 2`
    modsub=`echo $modver.0.0.0 | cut -d '.' -f 3`

    newver=`printf "%03i%03i%03i" "$maj" "$min" "$sub"`
    newmodver=`printf "%03i%03i%03i" "$modmaj" "$modmin" "$modsub"`

    errmsg="Requested $mod `printop $c` $v but version of $mod is $modver"
    trap 'echo $errmsg' EXIT
    case "$c" in
        lteq)   test "$newmodver" -le "$newver" || exit 2 ;;
        lt)     test "$newmodver" -lt "$newver" || exit 2 ;;
        eq)     test "$newmodver" -eq "$newver" || exit 2 ;;
        neq)    test "$newmodver" -ne "$newver" || exit 2 ;;
        gt)     test "$newmodver" -gt "$newver" || exit 2 ;;
        gteq)   test "$newmodver" -ge "$newver" || exit 2 ;;
    esac
    trap '' EXIT

    exit 0
}

parse_cmd_line $@

test "$_cflagsI$_cflagso$_libsl$_libsL$_libso$_modversion" = "nononononono" &&
    _exists=yes

if test "$_exists" = "yes" ; then
    exec 1>/dev/null 2>&1
    n=0
    while test "$n" != "$nmod" ; do
        mod=`eval echo \\$_mod_$n`
        file=`find_file $mod`
        test "$file" != "notfound" || exit 2
        check_constraints $n $file || exit 2
        n=`expr $n + 1`
    done
    exit 0
fi

if test "$_modversion" = "yes" ; then
    n=0
    while test "$n" != "$nmod" ; do
        mod=`eval echo \\$_mod_$n`
        file=`find_file $mod`
        test "$file" != "notfound" || exit 2
        _MODVERSIONS="$_MODVERSIONS `data_from_file Version $file`"
        n=`expr $n + 1`
    done
fi

requires="`all_requires`"
case "$requires" in
    *notfound*)     exit 2 ;;
esac
requires=`remove_doubles $requires`
if test "$_cflagsI" = "yes" -o "$_cflagso" = "yes"; then
    _CFLAGS=`collect_data Cflags $requires`
    _CFLAGS=`remove_doubles $_CFLAGS`
    test "$_cflagsI" = "yes" && _CFLAGSI=`extract I $_CFLAGS`
    test "$_cflagso" = "yes" && _CFLAGSo=`extract D $_CFLAGS`
fi
if test "$_libsl" = "yes" -o "$_libsL" = "yes" -o "$_libso" = "yes" ; then
    _LIBS=`collect_data Libs $requires`
    if test "$_plus_private" = "yes"; then
        _LIBSP=`collect_data Libs.private $requires`
    else
        _LIBSP=""
    fi
    _LIBS="$_LIBS $_LIBSP"
    _LIBS=`remove_doubles $_LIBS`
    test "$_libsL" = "yes" && _LIBSL=`extract L $_LIBS`
    test "$_libsl" = "yes" && _LIBSl=`extract l $_LIBS`
    test "$_libso" = "yes" && _LIBSo=`extract lo $_LIBS`
fi

for i in $_MODVERSIONS ; do
    echo $i
done

PRINT=no
LINE=""
test "$_cflagsI" = "yes" && LINE="$LINE $_CFLAGSI" && PRINT=yes
test "$_cflagso" = "yes" && LINE="$LINE $_CFLAGSo" && PRINT=yes
test "$_libsL" = "yes" && LINE="$LINE $_LIBSL" && PRINT=yes
test "$_libsl" = "yes" && LINE="$LINE $_LIBSl" && PRINT=yes
test "$_libso" = "yes" && LINE="$LINE $_LIBSo" && PRINT=yes
test "$PRINT" = "yes" && echo $LINE
