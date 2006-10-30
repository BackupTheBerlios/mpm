/* ------------------------------------------------------------------------- */

/*
 * mntrescue by Ivo van Poorten, Oct. 2006
 *
 * same as: mount /dev/rescue /usr
 *
 * Public Domain
 *
 */

/* ------------------------------------------------------------------------- */

#include <unistd.h>
#include <fcntl.h>

/* ------------------------------------------------------------------------- */

int mntrescue(void) {
    int r, fdout;
    if ((r = mount("/dev/rescue", "/usr", 0)) != 0)     return r;
    fdout = open("/etc/mtab", O_WRONLY | O_APPEND);
    write(fdout, "/dev/rescue /usr 3 rw\n", 22);
    close(fdout);
    return r;
}

/* ------------------------------------------------------------------------- */

#ifndef NO_MAIN
int main(int argc, char **argv) {
    if (argc != 1) {
        write(2,"usage: mntrescue\n", 17);
        _exit(-1);
    }
    if (mntrescue() != 0) {
        write(2, "mntrescue: unable to mount /dev/rescue on /usr\n", 47);
        return -1;
    }
    write(1,"/dev/rescue is read-write mounted on /mnt\n", 42);
    return 0;
}
#endif

/* ------------------------------------------------------------------------- */
