#define _POSIX_SOURCE
#define _MINIX

#include <minix/config.h>
#include <minix/const.h>
#include <minix/type.h>
#include <minix/ipc.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "gfx.h"
#include "gfx_ioctl.h"

PRIVATE gfx_request_set_mode_t mode;

PUBLIC int main(int argc, char **argv) {
    int fd, r;

    fd = open("/dev/gfx", O_RDWR);
    if (fd < 0 ) {
        fprintf(stderr, "cannot open /dev/gfx\n");
        _exit(-1);
    }

    mode = TEXT_COLOR;
    r = ioctl(fd, GFX_REQUEST_SET_MODE, &mode);
    close(fd);

    return 0;
}
