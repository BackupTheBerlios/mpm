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
PRIVATE gfx_request_line_t line;

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
    if (r<0) {
        fprintf(stderr, "unable to set videomode, error %d\n", errno);
        _exit(-1);
    }

    line.x1 = 0; line.x2 = 639; line.y1 = 175; line.c = 3;
    ioctl(fd, GFX_REQUEST_DRAW_LINE_HORI, &line);
    line.y1 = 0; line.c = 3;
    ioctl(fd, GFX_REQUEST_DRAW_LINE_HORI, &line);
    line.y1 = 349; line.c = 3;
    ioctl(fd, GFX_REQUEST_DRAW_LINE_HORI, &line);
    line.x1 = 0; line.y1 = 0; line.y2 = 349;
    ioctl(fd, GFX_REQUEST_DRAW_LINE_VERT, &line);
    line.x1 = 639;
    ioctl(fd, GFX_REQUEST_DRAW_LINE_VERT, &line);

    line.c = 4;
    for (x=0; x<=639; x+=32) {
        line.x1 = x;
        line.y1 = 0;
        line.x2 = 639-x;
        line.y2 = 349;
        ioctl(fd, GFX_REQUEST_DRAW_LINE, &line);
    }
    line.c = 5;
    for (y=349; y>=0; y-=32) {
        line.x1 = 0;
        line.y1 = y;
        line.x2 = 639;
        line.y2 = 349 - y;
        ioctl(fd, GFX_REQUEST_DRAW_LINE, &line);
    }

    sleep(2);

    mode = TEXT_COLOR;
    r = ioctl(fd, GFX_REQUEST_SET_MODE, &mode);
    close(fd);

    return 0;
}
