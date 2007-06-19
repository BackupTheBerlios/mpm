#! /bin/sh

keymaps=/usr/lib/keymaps

VERSION="2007.0 (beta)"
TERM=minix-color
eval DIALOGOPTS=\'--backtitle "\"               The MPMPORTS Minix Distribution, $VERSION\""\'
export TERM TERMCAP DIALOGOPTS

# -----------------------------------------------------------------------------

WELCOME="        Welcome to the MPMPORTS Minix distribution, $VERSION

     MPMPORTS Framework Copyright (C) 2006-2007 by Ivo van Poorten
   Minix 3.1.2a Copyright (C) 1986-2006 Vrije Universiteit, Amsterdam
The Vrije Universiteit DOES NOT actively endorse, support or affiliates
                 itself with this project in ANY WAY
"
ABOUT="/usr/lib/about.txt"

welcome() {
    r=""
    while test "$r" != "Install" ; do
        r=`dialog \
                --stdout \
                --begin 3 1 --infobox "$WELCOME" 8 76 \
                --and-widget \
                --no-cancel --begin 13 22 \
                --menu "What do you want to do?" 10 36 3 \
                "Install" "Install to harddisk" \
                "About" "About MPMPORTS" \
                "Exit" "Exit to shell" `
        case "$r" in
            Install)    : ;;
            About)
                dialog --no-cancel --begin 3 1 --textbox $ABOUT 20 76 
                ;;
            Exit)       exit 0 ;;
        esac
    done
}

# -----------------------------------------------------------------------------

set_keymap() {
    l=`ls $keymaps/*.map | grep -v us-std` 2>/dev/null
    items=""
    for i in $l ; do
        si=`basename $i .map`
        items="$items $si \"\" off"
    done
    r=""
    while test -z "$r" ; do
        r=`eval dialog --no-cancel --stdout \
                       --radiolist \"Please select a keymap\" 0 0 0 \
                       us-std \"\" on $items`
    done
    loadkeys $keymaps/$r.map
}

# -----------------------------------------------------------------------------

set_date() {
    Y=`date +%Y`
    M=`date +%m`
    D=`date +%d`
    r=""
    while test -z "$r" ; do
        r=`dialog --no-cancel --stdout \
                  --calendar "Please specify the current date" 0 0 $D $M $Y`
    done
    D=`echo $r | cut -d '/' -f 1`
    M=`echo $r | cut -d '/' -f 2`
    Y=`echo $r | cut -d '/' -f 3 | cut -c 3-4`
    date $M$D$Y`date +%H%M%S` >/dev/null
}

set_time() {
    h=`date +%H`
    m=`date +%M`
    s=`date +%S`
    r=""
    while test -z "$r" ; do
        r=`dialog --no-cancel --stdout \
                  --timebox "Please specify the current time" 0 0 $h $m $s`
    done
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
                  --menu "$DATE" 7 36 0 "Done" ""  \
                                        "Set date" "" \
                                        "Set time" "" \
                                        "Refresh" ""`
        case "$r" in
            *date) set_date ;;
            *time) set_time ;;
        esac
    done
}

# -----------------------------------------------------------------------------

at_or_bios_wini() {
    r=""
    while test -z "$r" ; do
        r=`dialog --no-cancel --stdout \
                  --radiolist "Please select an I/O driver" 0 0 0 \
                              "at_wini" "AT/IDE Driver" on \
                              "bios_wini" "BIOS Driver" off `
    done
    service up /sbin/$r -dev /dev/c0d0
}

# -----------------------------------------------------------------------------

allethdrivers="fxp dpeth rtl8139 dp8390 lance"

ethernet_chip() {
    r=""
    while test -z "$r" ; do
        r=`dialog --no-cancel --stdout \
                  --radiolist \
                  "Please select your ethernet card" 0 0 0 \
                  "Intel Pro/100" "" off \
                  "3Com 501 / 3Com 509 based card" "" off \
                  "Realtek 8139 based card" "" off \
                  "Realtek 8029 based card (use by Qemu)" "" off \
                  "NE2000, 3Com 503 or WD based card (used by Bochs)" "" off \
                  "AMD LANCE (used by VMware)" "" on `
    done

    case $r in
        Intel*) q="fxp@" ;;
        3Com**) q="dpeth@" ;;
        *8139*) q="rtl8139@" ;;
        *8029*) q="dp8390@DPETH0=pci" ;;
        NE200*) q="dp8390@DPETH0=240:9" ;;
        AMD*L*) q="lance@LANCE0=on" ;;
        *)      echo "This should not be possible. There's a bug in $0."
                exit 2 ;;
    esac

    ethname="$r"
    ethdriver=`echo "$q" | cut -d '@' -f 1`
    ethjustargs=`echo "$q" | cut -d '@' -f 2`
    ethargs="${ethdriver}_args='$ethjustargs'"
}

dhcp_or_manual() {
    r=""
    while test -z "$r" ; do
        r=`dialog --no-cancel --stdout \
                  --radiolist \
                  "Network Settings" 0 0 0 \
                  "Manual" "" on \
                  "DHCP" "" off `
    done
    test "$r" = "Manual" && netdhcp=0 || netdhcp=1
}

network_settings() {
    r=""
    while test -z "$r" ; do
        r=`dialog --no-cancel --stdout \
                  --form \
                  "Network Settings" 0 0 0 \
                  "IP Address" 1 0 "$netip"   1 15 15 15 \
                  "Netmask"    2 0 "$netmask" 2 15 15 15 \
                  "Nameserver" 3 0 "$netdns"  3 15 15 15 \
                  "Gateway"    4 0 "$netgw"   4 15 15 15 \
                  "MTU"        5 0 "$netmtu"  5 15  5  5 `
    done
    netip=`  echo "$r" | head -1`
    netmask=`echo "$r" | head -2 | tail -1`
    netdns=` echo "$r" | head -3 | tail -1`
    netgw=`  echo "$r" | head -4 | tail -1`
    netmtu=` echo "$r" | head -5 | tail -1`
}

kill_service_by_fullname() {
    tmp=`ps -ef | grep $1 | grep -v grep`
    pid=`echo $tmp | cut -d ' ' -f 4`
    test -n "$pid" && service down $pid
}

network_restart() {
    dialog --infobox "Preparing..." 3 40
    sleep 1

    echo "eth0 $ethdriver 0 { default; };" > /etc/inet.conf
    echo "127.0.0.1 localhost" >  /etc/hosts
    echo "$netdns %nameserver" >> /etc/hosts
    echo "minix-install" > /etc/hostname.file

    pid=`cat /usr/run/nonamed.pid 2>/dev/null`
    test -n "$pid" && kill $pid
    kill_service_by_fullname /sbin/inet

    for i in $allethdrivers ; do
        kill_service_by_fullname /sbin/$i
    done

    dialog --infobox "Bringing up networking..." 3 40
    sleep 1

    test -n "$ethjustargs" && args="-args $ethjustargs"
    service up /sbin/$ethdriver $args -period 5HZ
    service up /sbin/inet
    ifconfig -I /dev/ip -h $netip -n $netmask -m $netmtu
    add_route -I /dev/ip -g $netgw
    nonamed -L &
}

# -----------------------------------------------------------------------------

ethname="none"
ethdriver="none"
ethargs=""
ethjustargs=""
netdhcp=0
netip=192.168.0.
netmask=255.255.255.0
netdns=192.168.0.1
netgw=192.168.0.1
netmtu=1500
netup=no

welcome
set_keymap
time_and_date
at_or_bios_wini

while test "$netup" = "no" ; do
    ethernet_chip
    dhcp_or_manual
    test $netdhcp -eq 0 && network_settings

    network_restart

    ping $netdns >/dev/null 2>&1 && netup=yes

    if test "$netup" = "no" ; then
        dialog --ok-label "Retry" --msgbox "Cannot bring up networking!" 5 40
    else
        dialog --ok-label "Continue" --msgbox "Networking OK" 5 40
    fi
done

# -----------------------------------------------------------------------------

