#define _POSIX_SOURCE
#define _MINIX
#define _SYSTEM

#include <minix/config.h>
#include <minix/com.h>
#include <minix/type.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/const.h>
#include <ibm/int86.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define GFX_DRIVER

#include "gfx.h"
#include "gfx_ioctl.h"
#include "generic.h"

EXTERN int debug;

PRIVATE vir_bytes bios_buf_vir, bios_buf_size;
PRIVATE phys_bytes bios_buf_phys;
PRIVATE int current_mode;
PRIVATE char *myname;
PRIVATE struct reg86u reg86;
PRIVATE unsigned short width = 0, height = 0;
PRIVATE unsigned char bpp = 0;

PRIVATE struct mode_list_s {
    gfx_mode_t mode;
    unsigned char al;
    unsigned short width, height;
    unsigned char bpp;
} mode_list[] = {
    { TEXT_MONO,       0x02,  80,  25, 16 },
    { TEXT_COLOR,      0x03,  80,  25, 16 },
    { VGA_640x480x2,   0x11, 640, 480,  1 },
    { VGA_640x480x16,  0x12, 640, 480,  4 },
    { VGA_320x200x256, 0x13, 320, 200,  8 },
    { GFX_MODE_NONE,   0x00,   0,   0,  0 }
};

PRIVATE int init(char *name) {
    struct reg86u reg86;
    int r;

    myname = name;

    r = sys_getbiosbuffer(&bios_buf_vir, &bios_buf_size);
    if (r != OK) panic(myname, "sys_getbiosbuffer failed", r);
    DEBUG report(myname, "bios_buf_vir", bios_buf_vir);
    DEBUG report(myname, "bios_buf_size", bios_buf_size);
    r = sys_umap(SYSTEM, D, bios_buf_vir, (phys_bytes)bios_buf_size,
                                                            &bios_buf_phys);
    if (r != OK) panic(myname, "sys_umap failed", r);
    if (bios_buf_phys + bios_buf_size > 0x100000)
        panic(myname, "bad BIOS buffer, phys", bios_buf_phys);
    DEBUG report(myname, "bios_buf_phys", bios_buf_phys);

    current_mode = TEXT_COLOR;
    return 0;
}

PRIVATE int set_mode(gfx_mode_t mode) {
    int x, r;

    for (x=0; mode_list[x].al; x++) {
        if (mode == mode_list[x].mode) break;
    }
    if (mode_list[x].al == 0) return EGFX_UNSUPPORTED_MODE;
    reg86.u.b.ah = 0x00;
    reg86.u.b.al = mode_list[x].al;
    reg86.u.b.intno = 0x10;
    r = sys_int86(&reg86);
    if (r != OK) panic(myname, "BIOS call failed", r);

    current_mode = mode;

    width = mode_list[x].width;
    height = mode_list[x].height;
    bpp = mode_list[x].bpp;

    return 0;
}

PRIVATE int get_pixel(unsigned short x, unsigned short y, unsigned int *c) {

    if (x<0 || x>width) return EGFX_OUT_OF_RANGE;
    if (y<0 || y>height) return EGFX_OUT_OF_RANGE;

    reg86.u.b.ah = 0x0d;
    reg86.u.w.cx = x;
    reg86.u.w.dx = y;
    reg86.u.b.intno = 0x10;
    sys_int86(&reg86);
    *c = reg86.u.b.al;

    return 0;
}

PRIVATE int put_pixel(unsigned short x, unsigned short y, unsigned int c) {

    if (x<0 || x>width) return EGFX_OUT_OF_RANGE;
    if (y<0 || y>height) return EGFX_OUT_OF_RANGE;
    if (bpp == 1) c = !!c;

    reg86.u.b.ah = 0x0c;
    reg86.u.b.al = c;
    reg86.u.w.cx = x;
    reg86.u.w.dx = y;
    reg86.u.b.intno = 0x10;
    sys_int86(&reg86);

    return 0;
}

PRIVATE int clear_screen(void) {

    set_mode(current_mode);

    return 0;
}

PRIVATE int draw_line_hori(unsigned short x1, unsigned short y1,
                           unsigned short x2,
                           unsigned int c) {
    int tx;

    if (x1>x2) {
        tx=x1; x1=x2; x2=tx;
    }
    for (; x1<=x2; x1++) put_pixel(x1, y1, c);

    return 0;
}

PRIVATE int draw_line_vert(unsigned short x1, unsigned short y1,
                                              unsigned short y2,
                           unsigned int c) {
    int ty;

    if (y1>y2) {
        ty=y1; y1=y2; y2=ty;
    }
    for (; y1<=y2; y1++) put_pixel(x1, y1, c);

    return 0;
}

#undef ABS
#define ABS(x) ( ((x)<0) ? (-(x)) : (x) )

PRIVATE int draw_line(unsigned short x1, unsigned short y1,
                      unsigned short x2, unsigned short y2,
                      unsigned int c) {
    int dx, dy, i, e, x, y, t;

    dx = x2 - x1;
    dy = y2 - y1;

    c=(c&0xff)|0x0c00;
    if (ABS(dx) > ABS(dy)) {
        if (dx < 0) {
            int tx, ty;
            tx = x1;    ty = y1;
            x1 = x2;    y1 = y2;
            x2 = tx;    y2 = ty;
            dx = -dx;   dy = -dy;
        }
        if (dy < 0) {
            i = -1;
            dy = -dy;
        } else {
            i = 1;
        }
        e = dx >> 1;
        y = y1;
        for (x=x1; x<=x2; x++) {
            /* (e>=dx) --> TX=1 else TY=0 */
#define TX (((unsigned int)dx - (unsigned int)e - 1) >> 31)
            e = e - TX*dx + dy;
            y += TX*i;
            put_pixel(x,y,c);
        }
    } else {
        if (dy < 0) {
            int tx, ty;
            tx = x1;    ty = y1;
            x1 = x2;    y1 = y2;
            x2 = tx;    y2 = ty;
            dx = -dx;   dy = -dy;
        }
        if (dx < 0) {
            i = -1;
            dx = -dx;
        } else {
            i = 1;
        }
        e = dy >> 1;
        x = x1;
        for (y=y1; y<=y2; y++) {
#define TY (((unsigned int)dy - (unsigned int)e - 1) >> 31)
            e = e - TY*dy + dx;
            x += TY*i;
            put_pixel(x,y,c);
        }
    }

    return 0;
}

/* Unused, but here to avoid compile warnings */
PRIVATE int draw_rect(unsigned short x1, unsigned short y1,
                      unsigned short x2, unsigned short y2,
                      unsigned int c) {
    return -1;
}

/* Unused, but here to avoid compile warnings */
PRIVATE int put_char(unsigned short x, unsigned short y,
                     unsigned int c, unsigned char chr, gfx_font_t f) {
    return -1;
}

/* Unused, but here to avoid compile warnings */
PRIVATE int put_string(unsigned short x, unsigned short y,
                       unsigned int c, unsigned char *str,
                       unsigned int len, gfx_font_t f) {
    return -1;
}

PUBLIC gfx_funcs_t gfx_funcs_vga_bios = {
    "vga_bios",
    TEXT_MONO | TEXT_COLOR | VGA_640x480x2 | VGA_640x480x16 | VGA_320x200x256,
    init,
    set_mode,
    get_pixel,
    put_pixel,
    clear_screen,
    draw_line,
    draw_line_hori,
    draw_line_vert,
    generic_draw_rect,
    generic_put_char,
    generic_put_string
};
