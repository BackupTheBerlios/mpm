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
PRIVATE gfx_request_pixel_t pixel;

PUBLIC int main(int argc, char **argv) {
    int fd, r;
    int x, y, c1, c2;

    fd = open("/dev/gfx", O_RDWR);
    if (fd < 0 ) {
        fprintf(stderr, "cannot open /dev/gfx\n");
        _exit(-1);
    }

    mode = EGA_640x350x16;
    r = ioctl(fd, GFX_REQUEST_SET_MODE, &mode);
    fprintf(stderr, "r=%d\n", r);
    if (r<0) {
        fprintf(stderr, "unable to set videomode, error %d\n", errno);
        _exit(-1);
    }

    pixel.c=2;
    for (y=0; y<100; y+=16) {
        pixel.y = y;
        for (x=0; x<160; x+=16) {
            pixel.x = x;
            ioctl(fd, GFX_REQUEST_PUT_PIXEL, &pixel);
        }
    }

    sleep(2);

    ioctl(fd, GFX_REQUEST_CLEAR_SCREEN, NULL);

    pixel.x = pixel.y = 0;
    ioctl(fd, GFX_REQUEST_GET_PIXEL, &pixel);
    c1 = pixel.c;
    pixel.x++;
    ioctl(fd, GFX_REQUEST_GET_PIXEL, &pixel);
    c2 = pixel.c;

    mode = TEXT_COLOR;
    ioctl(fd, GFX_REQUEST_SET_MODE, &mode);

    printf("c1 = %d    c2 = %d\n", c1, c2);

    close(fd);

    return 0;
}
