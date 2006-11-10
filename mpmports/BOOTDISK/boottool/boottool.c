/* ------------------------------------------------------------------------- */

/*
 * boottool
 *
 * copyright (c) 2006 Ivo van Poorten
 *
 */

/* ------------------------------------------------------------------------- */

#include <unistd.h>

/* ------------------------------------------------------------------------- */

int uprescue(void);
int mntrescue(void);
int bzcp(char *, char *);

/* ------------------------------------------------------------------------- */

#define NORMAL "\033[0m"
#define RED    "\033[31m"
#define GREEN  "\033[32m"

/* ------------------------------------------------------------------------- */

static char *cmd[] = { "/usr/bin/sh", "/usr/etc/rc", NULL };
static char *nullenv[] = { NULL };

/* ------------------------------------------------------------------------- */

int main(int argc, char **argv) {
    write(1, "\nStarting /dev/rescue, size 4096kB: " , 36);
    if (uprescue() != 0)    goto failed;
    write(1, GREEN "ok\n" NORMAL "Uncompressing image to /dev/rescue: ", 48);
    if (bzcp("/usr.bz2", "/dev/rescue") != 0)   goto failed;
    write(1, GREEN "ok\n" NORMAL "Mounting /dev/rescue on /usr: ", 42);
    if (mntrescue() != 0)   goto failed;
    write(1, GREEN "ok\n" NORMAL "Starting sh /usr/etc/rc: ", 37);
    execve(cmd[0], cmd, nullenv); /* should not return */
failed:
    write(2, RED "failed\n" NORMAL, 16);
    return -1;
}

/* ------------------------------------------------------------------------- */
