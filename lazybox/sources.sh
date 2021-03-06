# If you want to build a smaller lazybox with only a few applications,
# you can edit SOURCES below. If you do not include 'cp', you should
# comment out CPLINKS as well!

SOURCES="add_route arp at backup banner basename cal calendar cat cdiff \
         cgrep chmem chmod chown ci cksum cleantmp cmp co comm \
         compress cp crc cut date dd decomp16 devsize dhrystone \
         diff dirname du ed eject env expand factor fgrep file find \
         fix fold fortune getty grep head hostaddr id ifdef \
         install intr irdpd isoread join kill last leave loadramdisk \
         login look lp lpd ls mail man mesg mkdir mkfifo mknod \
         mkproto mt newroot nm nonamed od passwd paste pr \
         pr_routes prep printenv printf printroot progressbar proto \
         pwd pwdauth rarpd rawspeed rdate readall remsync rget rlogin \
         rmdir badblocks cdprobe df mkfs mkswap mount readfs sed shar \
         size sleep slip sort split stat strings strip stty su sum swapfs \
         sync sysenv tail tar tcpd tcpstat tee term termcap tget time \
         touch tr treecmp tsort tty udpstat umount uname unexpand uniq \
         update uud uue vol wc which who whoami write xargs \
         yes fsck chroot dev2name finger host ifconfig modem nice ping \
         rcp rev rsh ramdisk synctree truncate writeisofs gomoku life top \
         ttt in.fingerd in.rshd"

CPLINKS="ln rm mv cpdir clone"
