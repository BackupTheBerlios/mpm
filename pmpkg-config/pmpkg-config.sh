#! /bin/sh

# Poor Man's Package Config
#
# Copyright (c) 2006 by Ivo van Poorten
# All rights reserved.
#
# License: GNU General Public License version 2.0
# (and NOT at your option any later version)
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# set -v

VERSION=0.90

test "$#" = "0" && echo "Must specify package names on the command line" &&
    exit 2

case " $@" in
    *\ --help*|*\ -\?)
    cat >&2 << __EOF__
Poor Man's pkg-config $VERSION -- Copyright (c) 2006 by Ivo van Poorten
Usage: $0 [OPTION...]
  --version                               output version of $0
  --modversion                            output version for package
  --atleast-pkgconfig-version=VERSION     require given version of pkg-config
                                          (always exits with 0)
  --libs                                  output all linker flags
  --static                                output linker flags for static
                                          linking
  --short-errors                          print short errors
  --libs-only-l                           output -l flags
  --libs-only-other                       output other libs (e.g. -pthread)
  --libs-only-L                           output -L flags
  --cflags                                output all pre-processor and compiler
                                          flags
  --cflags-only-I                         output -I flags
  --cflags-only-other                     output cflags not covered by the
                                          cflags-only-I option
  --variable=VARIABLENAME                 get the value of a variable
  --define-variable=VARIABLENAME=VALUE    set the value of a variable
  --exists                                return 0 if the module(s) exist
  --atleast-version=VERSION               return 0 if the module is at least
                                          version VERSION
  --exact-version=VERSION                 return 0 if the module is at exactly
                                          version VERSION
  --max-version=VERSION                   return 0 if the module is at most
                                          version VERSION
  --list-all                              list all known packages
  --debug                                 show verbose debug information
  --print-errors                          show verbose information about
                                          missing packages
  --silence-errors                        do not show verbose information
                                          about missing packages
  --errors-to-stdout                      print errors from --print-errors
                                          to stdout and not to stderr
  -?, --help                              show this help message
  --usage                                 display brief usage message
__EOF__
    exit 2 ;;
    *\ --usage*)
    cat >&2 << __EOF__
Usage: $0 [-?] [--version] [--modversion]
        [--atleast-pkgconfig-version=VERSION] [--libs] [--static]
        [--short-errors] [--libs-only-l] [--libs-only-other] [--libs-only-L]
        [--cflags] [--cflags-only-I] [--cflags-only-other]
        [--exists] [--debug] [--help] [--usage] [--print-errors]
        [--silence-errors] [--errors-to-stdout] [--variable=VARIABLENAME]
        [--define-variable=VARIABLENAME=VALUE] [--atleast-version=VERSION]
        [--exact-version=VERSION] [--max-version=VERSION] [--list-all]
__EOF__
    exit 2 ;;
esac

case "$@" in
    *--version*)      echo "$VERSION" ; exit 0 ;;
    *--atleast-pkgconfig-version*)      exit 0 ;;
esac

_plus_private=no
_verbose=normal
_tostdout=no

_listall=no
_exists=no
_cflagsI=no
_cflagso=no
_libsl=no
_libsL=no
_libso=no
_modversion=no
_variable=""
_defenv=""
_defvars=""
_atleastv=""
_exactv=""
_maxv=""

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
            --list-all)             _listall=yes ;;
            --exists)               _exists=yes ;;
            --atleast-version=*)    _atleastv=`echo $1 | cut -d '=' -f 2` ;;
            --exact-version=*)      _exactv=`echo $1 | cut -d '=' -f 2` ;;
            --max-version=*)        _maxv=`echo $1 | cut -d '=' -f 2` ;;
            --cflags)               _cflagsI=yes _cflagso=yes ;;
            --cflags-only-I)        _cflagsI=yes _cflagso=no ;;
            --cflags-only-other)    _cflagsI=no  _cflagso=yes ;;
            --libs)                 _libsl=yes _libsL=yes _libso=yes  ;;
            --libs-only-l)          _libsl=yes _libsL=no  _libso=no   ;;
            --libs-only-L)          _libsl=no  _libsL=yes _libso=no  ;;
            --libs-only-other)      _libsl=no  _libsL=no  _libso=yes ;;
            --static)               _plus_private=yes ;;
            --modversion)           _modversion=yes ;;
            --short-errors)         ;;
            --debug)                ;;
            --print-errors)         _verbose=all ;;
            --silence-errors)       _verbose=none ;;
            --errors-to-stdout)     _tostdout=yes ;;
            --variable=*)           _variable=`echo $1 | cut -d '=' -f 2` ;;
            --define-variable=*=*)
                _tmpenv=`echo "$1" | sed -e 's/[^=]*=\(.*\)/\1/'` 
                _defenv="$_defenv $_tmpenv"
                _tmpvar=`echo "$1" | cut -d '=' -f 2`
                _defvars="$_defvars $_tmpvar"
                ;;
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

all_requires2() {
    :
}

add_quotes() {
    sed -e 's/\(.*\)=\(.*\)/\1="\2"/'
}

remove_defined() {
    test -z "$_defvar" && cat && exit 0
    for i in $_defvars ; do
        sed -e "/^$i/d"
    done
}

data_from_file() {
    field=$1
    file=$2
    env=`sed -e '/^#.*$/d' -e '/^[ABCDEFGHIJKLMNOPQRSTUVWXYZ][ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_\.]*:/d' $file | remove_defined | add_quotes`
    flags=`cat $file | grep "^$field:" | cut -d ':' -f 2`
    output=`eval "eval $_defenv; $env "; eval echo $flags`
    echo $output
}

var_from_file() {
    var=$1
    file=$2
    env=`sed -e '/^#.*$/d' -e '/^[ABCDEFGHIJKLMNOPQRSTUVWXYZ][ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_\.]*:/d' $file | remove_defined | add_quotes`
    output=`eval "eval $_defenv; $env "; eval echo \$\{$var\}`
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
check_constraint_n() {
    mod=`eval echo \\$_mod_$1`
    c=`eval echo \\$_mod_$1_c`
    v=`eval echo \\$_mod_$1_v`

    if test "$c" = "ok" ; then
        echo ok
        exit 0
    fi

    check_constraint $mod $c $v $file
}

check_constraint() {
    mod=$1
    c=$2
    v=$3
    f=$4

    maj=`echo $v.0.0.0 | cut -d '.' -f 1`
    min=`echo $v.0.0.0 | cut -d '.' -f 2`
    sub=`echo $v.0.0.0 | cut -d '.' -f 3`

    modver=`data_from_file Version $f`
    modmaj=`echo $modver.0.0.0 | cut -d '.' -f 1`
    modmin=`echo $modver.0.0.0 | cut -d '.' -f 2`
    modsub=`echo $modver.0.0.0 | cut -d '.' -f 3`

    newver=`printf '%03d%03d%03d' "$maj" "$min" "$sub"`
    newmodver=`printf '%03d%03d%03d' "$modmaj" "$modmin" "$modsub"`

    errmsg="Requested $mod `printop $c` $v but version of $mod is $modver"
    trap 'echo $errmsg >&2' 0
    case "$c" in
        lteq)   test "$newmodver" -le "$newver" || exit 2 ;;
        lt)     test "$newmodver" -lt "$newver" || exit 2 ;;
        eq)     test "$newmodver" -eq "$newver" || exit 2 ;;
        neq)    test "$newmodver" -ne "$newver" || exit 2 ;;
        gt)     test "$newmodver" -gt "$newver" || exit 2 ;;
        gteq)   test "$newmodver" -ge "$newver" || exit 2 ;;
    esac
    trap '' 0
    echo ok
}

parse_cmd_line $@

if test "$_listall" = "yes" ; then
    for i in /usr/lib/pkgconfig /usr/local/lib/pkgconfig `IFS=":"; echo $PKG_CONFIG_PATH` ; do
        for j in $i/*.pc ; do
            test -f "$j" || continue
            k=`basename $j .pc`
            m=`grep "^Description:" $j | cut -d ':' -f 2`
            printf '%-19s %s\n' "$k" "$m"
        done
    done
    exit 0
fi

if test "$_tostdout" = "yes" ; then
    exec 2>&1
fi
if test "$_verbose" = "none" ; then
    exec 1>/dev/null 2>&1
fi

if test -n "$_atleastv" -o -n "$_exactv" -o -n "$_maxv" ; then
    _exists=yes
    test -n "$_atleastv" && c=gteq && ver="$_atleastv"
    test -n "$_exactv"   && c=eq   && ver="$_exactv"
    test -n "$_maxv"     && c=lteq && ver="$_maxv"
    n=0
    while test "$n" != "$nmod" ; do
        eval _mod_${n}_c=$c
        eval _mod_${n}_v=$ver
        n=`expr $n + 1`
    done
fi

test "$_cflagsI$_cflagso$_libsl$_libsL$_libso$_modversion" = "nononononono" \
     -a -z "$_variable" && _exists=yes

test "$_cflagsI$_cflagso$_libsl$_libsL$_libso" != "nonononono" && \
    _variable=""

if test "$_exists" = "yes" ; then
    if test "$_verbose" != "all" ; then
        exec 1>/dev/null 2>&1
    fi
    n=0
    while test "$n" != "$nmod" ; do
        mod=`eval echo \\$_mod_$n`
        file=`find_file $mod`
        test "$file" != "notfound" || exit 2
        ret=`check_constraint_n $n $file`
        test "$ret" = "ok" || exit 2
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
        ret=`check_constraint_n $n $file`
        test "$ret" = "ok" || exit 2
        n=`expr $n + 1`
    done
    for i in $_MODVERSIONS ; do
        echo $i
    done
fi

if test -n "$_variable" ; then
    n=0
    while test "$n" != "$nmod" ; do
        mod=`eval echo \\$_mod_$n`
        file=`find_file $mod`
        test "$file" != "notfound" || exit 2
        _VARIABLES="$_VARIABLES `var_from_file $_variable $file`"
        ret=`check_constraint_n $n $file`
        test "$ret" = "ok" || exit 2
        n=`expr $n + 1`
    done
    echo $_VARIABLES
    exit 0
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

PRINT=no
LINE=""
test "$_cflagsI" = "yes" && LINE="$LINE $_CFLAGSI" && PRINT=yes
test "$_cflagso" = "yes" && LINE="$LINE $_CFLAGSo" && PRINT=yes
test "$_libsL" = "yes" && LINE="$LINE $_LIBSL" && PRINT=yes
test "$_libsl" = "yes" && LINE="$LINE $_LIBSl" && PRINT=yes
test "$_libso" = "yes" && LINE="$LINE $_LIBSo" && PRINT=yes
test "$PRINT" = "yes" && echo $LINE

exit 0
