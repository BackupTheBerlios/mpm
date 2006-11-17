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
#include "vga_regs.h"

PRIVATE gfx_request_set_mode_t mode;
PRIVATE gfx_request_pixel_t pixel;
PRIVATE gfx_request_line_t line;
PRIVATE vga_registers_t vgaregs;

PUBLIC int main(int argc, char **argv) {
    int fd, r;
    int x, y, c1, c2, i;

    fd = open("/dev/gfx", O_RDWR);
    if (fd < 0 ) {
        fprintf(stderr, "cannot open /dev/gfx\n");
        _exit(-1);
    }

    mode = EGA_640x350x2;
    r = ioctl(fd, GFX_REQUEST_SET_MODE, &mode);
    if (r<0) {
        fprintf(stderr, "unable to set videomode, error %d\n", errno);
        _exit(-1);
    }

    ioctl(fd, GFX_REQUEST_CLEAR_SCREEN, &mode);

    pixel.c = 3;
    for (x=16; x<=639; x+=32) {
        pixel.x = x;
        for (y=16; y<=479; y+=32) {
            pixel.y = y;
            ioctl(fd, GFX_REQUEST_PUT_PIXEL, &pixel);
        }
    }

    line.x1 = 0; line.x2 = 639; line.y1 = 240; line.c = 2;
    ioctl(fd, GFX_REQUEST_DRAW_LINE_HORI, &line);
    line.y1 = 0; line.c = 2;
    ioctl(fd, GFX_REQUEST_DRAW_LINE_HORI, &line);
    line.y1 = 479; line.c = 2;
    ioctl(fd, GFX_REQUEST_DRAW_LINE_HORI, &line);
    line.x1 = 0; line.y1 = 0; line.y2 = 479;
    ioctl(fd, GFX_REQUEST_DRAW_LINE_VERT, &line);
    line.x1 = 639;
    ioctl(fd, GFX_REQUEST_DRAW_LINE_VERT, &line);

    line.c = 1;
    for (x=0; x<=639; x+=32) {
        line.x1 = x;
        line.y1 = 0;
        line.x2 = 639-x;
        line.y2 = 479;
        ioctl(fd, GFX_REQUEST_DRAW_LINE, &line);
    }
    line.c = 2;
    for (y=479; y>=0; y-=32) {
        line.x1 = 0;
        line.y1 = y;
        line.x2 = 639;
        line.y2 = 479 - y;
        ioctl(fd, GFX_REQUEST_DRAW_LINE, &line);
    }

    ioctl(fd, GFX_REQUEST_DUMP_REGISTERS, &vgaregs);

    sleep(2);

    r = ioctl(fd, GFX_REQUEST_RESET, NULL);

#define PRINT_RANGE(num, ary) \
    printf("{ "); \
    for (i=0; i<num-1; i++) { \
        printf("0x%02X, ", vgaregs.ary[i]); \
    } \
    printf("0x%02X },\n", vgaregs.ary[num-1]);

    PRINT_RANGE(1, misc);
    PRINT_RANGE(VGA_NUM_SEQ_REGS, seq);
    PRINT_RANGE(VGA_NUM_CRTC_REGS, crtc);
    PRINT_RANGE(VGA_NUM_GC_REGS, gc);
    PRINT_RANGE(VGA_NUM_AC_REGS, ac);

    close(fd);

    return 0;
}
