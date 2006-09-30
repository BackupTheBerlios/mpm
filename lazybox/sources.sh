
# missing because they need kernel headers:
# badblocks cdprobe df

# missing because of multiline function declarations: (mainly main)
# chroot dev2name finger host ifconfig

# because of curses:
# gomoku

# because of dots in filename:
# in.fingerd in.rshd

# what's up with fsck and fsck1?

SOURCES="add_route arp at backup banner basename cal calendar cat cdiff \
         cgrep chmem chmod chown ci cksum cleantmp cmp co comm \
         compress cp crc cut date dd decomp16 devsize dhrystone \
         diff dirname du ed eject env expand factor fgrep file find \
         fix fold fortune getty grep head hostaddr id ifdef \
         install intr irdpd isoread join ls"

CPLINKS="ln rm mv cpdir clone"
