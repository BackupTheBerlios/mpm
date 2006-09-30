
# missing because they need kernel or server headers:
# badblocks cdprobe df mkfs mkswap mount readfs

# missing because of multiline function declarations: (mainly main)
# chroot dev2name finger host ifconfig modem nice ping rcp rev rsh ramdisk

# because of curses:
# gomoku life

# because of dots in filename:
# in.fingerd in.rshd

# what's up with fsck and fsck1?

SOURCES="add_route arp at backup banner basename cal calendar cat cdiff \
         cgrep chmem chmod chown ci cksum cleantmp cmp co comm \
         compress cp crc cut date dd decomp16 devsize dhrystone \
         diff dirname du ed eject env expand factor fgrep file find \
         fix fold fortune getty grep head hostaddr id ifdef \
         install intr irdpd isoread join kill last leave loadramdisk \
         login look lp lpd ls mail man mesg mkdir mkfifo mknod \
         mkproto mt newroot nm nonamed od passwd paste pr \
         pr_routes prep printenv printf printroot progressbar proto \
         pwd pwdauth rarpd rawspeed rdate readall \
         remsync rget rlogin rmdir"

CPLINKS="ln rm mv cpdir clone"
