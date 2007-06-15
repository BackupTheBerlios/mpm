#! /bin/sh

keymaps=/usr/lib/keymaps

#DEBUG="echo DEBUG:"
VERSION="2007.0 (beta)"
TERM=minix-color
eval DIALOGOPTS=\'--backtitle "\"               The MPMPORTS Minix Distribution, $VERSION\""\'
export TERM TERMCAP DIALOGOPTS

# -----------------------------------------------------------------------------

welcome() {
    dialog --begin 3 1 --msgbox "
Welcome to the MPMPORTS Minix distribution, $VERSION

Not much to say here at the moment, so let's continue with the installation.
" 20 76
}

# -----------------------------------------------------------------------------

set_keymap() {
    l=`ls $keymaps/*.map | grep -v us-std` 2>/dev/null
    items=""
    for i in $l ; do
        si=`basename $i .map`
        items="$items $si \"\" off"
    done
    r=`eval dialog --no-cancel --stdout \
              --radiolist \"Please select a keymap\" 0 0 0 \
                          us-std \"\" on \
                          $items`
    loadkeys $keymaps/$r.map
}

# -----------------------------------------------------------------------------

set_date() {
    Y=`date +%Y`
    M=`date +%m`
    D=`date +%d`
    r=`dialog --no-cancel --stdout \
              --calendar "Please specify the current date" 0 0 $D $M $Y`
    D=`echo $r | cut -d '/' -f 1`
    M=`echo $r | cut -d '/' -f 2`
    Y=`echo $r | cut -d '/' -f 3 | cut -c 3-4`
    date $M$D$Y`date +%H%M%S` >/dev/null
}

set_time() {
    h=`date +%H`
    m=`date +%M`
    s=`date +%S`
    r=`dialog --no-cancel --stdout \
              --timebox "Please specify the current time" 0 0 $h $m $s`
    h=`echo $r | cut -d ':' -f 1`
    m=`echo $r | cut -d ':' -f 2`
    s=`echo $r | cut -d ':' -f 3`
    date $h$m$s >/dev/null
}

time_and_date() {
    r=""
    while test "$r" != "Done" ; do
        DATE=`date`
        r=`dialog --no-cancel --stdout \
                  --menu "$DATE" 7 36 0 "Refresh" ""  \
                                        "Set date" "" \
                                        "Set time" "" \
                                        "Done" ""`
        case "$r" in
            *date) set_date ;;
            *time) set_time ;;
        esac
    done
}

# -----------------------------------------------------------------------------

at_or_bios_wini() {
    r=`dialog --no-cancel --stdout \
              --radiolist "Please select an I/O driver" 0 0 0 \
                          "at_wini" "AT/IDE Driver" on \
                          "bios_wini" "BIOS Driver" off `
    $DEBUG service up /sbin/$r -dev /dev/c0d0
}

# -----------------------------------------------------------------------------

ethernet_chip() {
    r=`dialog --no-cancel --stdout \
              --radiolist \
              "Please select your ethernet card" 0 0 0 \
              "0" "No networking" off \
              "1" "Intel Pro/100" off \
              "2" "3Com 501 / 3Com 509 based card" off \
              "3" "Realtek 8139 based card" off \
              "4" "Realtek 8029 based card (use by Qemu)" off \
              "5" "NE2000, 3Com 503 or WD based card (used by Bochs)" off \
              "6" "AMD LANCE (used by VMware)" on `

    case $r in
        1)  q="fxp@" ;;
        2)  q="dpeth@" ;;
        3)  q="rtl8139@" ;;
        4)  q="dp8390@DPETH0=pci" ;;
        5)  q="dp8390@DPETH0=240:9" ;;
        6)  q="lance@" ;;
        *)  q="none@" ;;
    esac

    ethdriver=`echo "$q" | cut -d '@' -f 1`
    ethargs="${ethdriver}_args='"`echo "$q" | cut -d '@' -f 2`"'"
}

# -----------------------------------------------------------------------------

ethdriver="none"
ethargs=""

welcome
set_keymap
time_and_date
at_or_bios_wini
ethernet_chip

# -----------------------------------------------------------------------------

