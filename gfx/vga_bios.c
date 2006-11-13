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

#include "gfx.h"
#include "gfx_ioctl.h"

EXTERN int debug;

PRIVATE vir_bytes bios_buf_vir, bios_buf_size;
PRIVATE phys_bytes bios_buf_phys;
PRIVATE int current_mode;
PRIVATE char *myname;

PRIVATE struct mode_list_s {
    gfx_mode_t mode;
    unsigned char al;
} mode_list[] = {
    { TEXT_MONO,       0x02 },
    { TEXT_COLOR,      0x03 },
    { EGA_320x200x16,  0x0d },
    { EGA_640x350x16,  0x10 },
    { VGA_640x480x16,  0x12 },
    { VGA_320x200x256, 0x13 },
    { GFX_MODE_NONE,   0x00 }
};

PRIVATE void int10h(struct reg86u *reg86) {
    int r;

    reg86->u.b.intno = 0x10;
    r = sys_int86(reg86);
    if (r != OK) panic(myname, "BIOS call failed", r);
}

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
    struct reg86u reg86;
    int x;

    for (x=0; mode_list[x].al; x++) {
        if (mode == mode_list[x].mode) break;
    }
    if (mode_list[x].al == 0) return EGFX_UNSUPPORTED_MODE;
    reg86.u.b.ah = 0x00;
    reg86.u.b.al = mode_list[x].al;
    int10h(&reg86);

    current_mode = mode;

    return 0;
}

PRIVATE int get_pixel(int x, int y, int *c) {
    struct reg86u reg86;

    reg86.u.b.ah = 0x0d;
    reg86.u.w.cx = x;
    reg86.u.w.dx = y;
    int10h(&reg86);
    *c = reg86.u.b.al;

    return 0;
}

PRIVATE int put_pixel(int x, int y, int c) {
    struct reg86u reg86;

    reg86.u.b.ah = 0x0c;
    reg86.u.b.al = c;
    reg86.u.w.cx = x;
    reg86.u.w.dx = y;
    int10h(&reg86);

    return 0;
}

PRIVATE int clear_screen(void) {

    set_mode(current_mode);

    return 0;
}

PRIVATE int draw_line_hori(int x1, int y1, int x2, int c) {
    int tx;
    struct reg86u reg86;

    if (x1>x2) {
        tx=x1; x1=x2; x2=tx;
    }
    c=(c&0xff)|0x0c00;
    for (; x1<=x2; x1++) {
        reg86.u.w.ax = c;
        reg86.u.w.dx = y1;
        reg86.u.w.cx = x1;
        int10h(&reg86);
    }

    return 0;
}

PRIVATE int draw_line_vert(int x1, int y1, int y2, int c) {
    int ty;
    struct reg86u reg86;

    if (y1>y2) {
        ty=y1; y1=y2; y2=ty;
    }
    c=(c&0xff)|0x0c00;
    for (; y1<=y2; y1++) {
        reg86.u.w.ax = c;
        reg86.u.w.cx = x1;
        reg86.u.w.dx = y1;
        int10h(&reg86);
    }

    return 0;
}

#undef ABS
#define ABS(x) ( ((x)<0) ? (-(x)) : (x) )

PRIVATE int draw_line(int x1, int y1, int x2, int y2, int c) {
    int dx, dy, i, e, x, y;

    dx = x2 - x1;
    dy = y2 - y1;

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
        e = dx / 2;
        y = y1;
        for (x=x1; x<=x2; x++) {
            if (e >= dx) {
                e -= dx;
                y += i;
            }
            e += dy;
            put_pixel(x, y, c);
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
        e = dy / 2;
        x = x1;
        for (y=y1; y<=y2; y++) {
            if (e >= dy) {
                e -= dy;
                x += i;
            }
            e += dx;
            put_pixel(x, y, c);
        }
    }

    return 0;
}

PUBLIC gfx_funcs_t gfx_funcs_vga_bios = {
    "vga_bios",
    TEXT_MONO | TEXT_COLOR | EGA_320x200x16 | EGA_640x350x16 | VGA_640x480x16 |
    VGA_320x200x256,
    init,
    set_mode,
    get_pixel,
    put_pixel,
    clear_screen,
    draw_line,
    draw_line_hori,
    draw_line_vert
};

PUBLIC gfx_funcs_t gfx_funcs_ega_bios = {
    "ega_bios",
    TEXT_MONO | TEXT_COLOR | EGA_320x200x16 | EGA_640x350x16,
    init,
    set_mode,
    get_pixel,
    put_pixel,
    clear_screen,
    draw_line,
    draw_line_hori,
    draw_line_vert
};
