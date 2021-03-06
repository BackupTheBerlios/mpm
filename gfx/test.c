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
PRIVATE gfx_request_rect_t rect;
PRIVATE gfx_request_char_t chr;
PRIVATE gfx_request_string_t str;
PRIVATE vga_registers_t vgaregs;

PRIVATE const char *qbf = "The quick brown fox jumps over the lazy dog";
PRIVATE const char *pwl = "Pa's wijze lynx bezag vroom het fikse aquaduct";

PUBLIC int main(int argc, char **argv) {
    int fd, r;
    int x, y, c1, c2, i;

    fd = open("/dev/gfx", O_RDWR);
    if (fd < 0 ) {
        fprintf(stderr, "cannot open /dev/gfx\n");
        _exit(-1);
    }

    mode = VGA_640x480x16;
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

    chr.f = GFX_FONT_8x8; chr.c = 6;
    chr.y = 13; chr.x = 13;
    for (i=0; i<256; i++) {
        chr.chr = i;
        ioctl(fd, GFX_REQUEST_PUT_CHAR, &chr);
        chr.x += 8;
        if (chr.x > 600) {
            chr.x = 13;
            chr.y += 8;
        }
    }

    chr.f = GFX_FONT_8x16; chr.c = 7;
    chr.y = 87; chr.x = 13;
    for (i=0; i<256; i++) {
        chr.chr = i;
        ioctl(fd, GFX_REQUEST_PUT_CHAR, &chr);
        chr.x += 8;
        if (chr.x > 600) {
            chr.x = 13;
            chr.y += 16;
        }
    }

    str.f = GFX_FONT_8x16; str.c = 9;
    str.x = 13; str.s = (unsigned char*) qbf; str.len = strlen(qbf);
    for (i=0; i<8; i++) {
        str.y = 270 + i*16;
        ioctl(fd, GFX_REQUEST_PUT_STRING, &str);
        str.x++;
    }

    str.s = (unsigned char*) pwl; str.len = strlen(pwl);
    str.x = 13; str.y = 416; str.c = 1;
    for (i=0; i<15; i++) {
        ioctl(fd, GFX_REQUEST_PUT_STRING, &str);
        str.c++; str.x++; str.y++;
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

    rect.c = 4;
    for (x=0, y=0; x<=319; x+=32, y+=24) {
        rect.x1 = x;
        rect.x2 = 639 - x;
        rect.y1 = y;
        rect.y2 = 479 - y;
        ioctl(fd, GFX_REQUEST_DRAW_RECT, &rect);
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
