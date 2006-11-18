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
#include <sys/vm.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define GFX_DRIVER

#include "gfx.h"
#include "gfx_ioctl.h"
#include "vga_raw.h"
#include "generic.h"

#define FB_BASE 0xa0000

EXTERN int debug;

PRIVATE char *myname;

PRIVATE unsigned char *fb, *afb, *curfb;
PRIVATE unsigned short width = 0, height = 0, stride = 0;
PRIVATE unsigned char bpp = 0, planar = 0;

#define OUTB(p,v) sys_outb(p,v)
#define INB(p,v)  sys_inb(p,v)

PRIVATE struct mode_list_s {
    gfx_mode_t mode;
    unsigned int fb_off;
    unsigned char bpp, planar;
    unsigned short width, height;
    vga_raw_mode_t raw_mode;
} mode_list[] = {
    /* from sample modes.c */
    { TEXT_COLOR, 0xb0000 - FB_BASE, 16, 0, 80, 24,
      { { 0x67 },
        { 0x03, 0x00, 0x03, 0x00, 0x02 },
        { 0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F, 0x00, 0x4F, 0x0D,
          0x0E, 0x00, 0x00, 0x00, 0x50, 0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96,
          0xB9, 0xA3, 0xFF },
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00, 0xFF },
        { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07, 0x38, 0x39, 0x3A,
          0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x0C, 0x00, 0x0F, 0x08, 0x00 } } },
    /* regdump from vga_bios driver */
    { VGA_640x480x2, 0xa0000 - FB_BASE, 1, 0, 640, 480,
      { { 0xE3 },
        { 0x03, 0x01, 0x0F, 0x00, 0x06 },
        { 0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E, 0x00, 0x40, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0xEA, 0x8C, 0xDF, 0x28, 0x00, 0xE7,
          0x04, 0xC3, 0xFF },
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x01, 0xFF },
        { 0x00, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x3F,
          0x3F, 0x3F, 0x3F, 0x3F, 0x3F, 0x01, 0x00, 0x0F, 0x00, 0x00 } } },
    /* regdump from vga_bios driver */
    { VGA_640x480x16, 0xa0000 - FB_BASE, 4, 1, 640, 480,
      { { 0xE3 },
        { 0x03, 0x01, 0x0F, 0x00, 0x06 },
        { 0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E, 0x00, 0x40, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0xEA, 0x8C, 0xDF, 0x28, 0x00, 0xE7,
          0x04, 0xE3, 0xFF },
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0F, 0xFF },
        { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07, 0x38, 0x39, 0x3A,
          0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x01, 0x00, 0x0F, 0x00, 0x00 } } },
#if 0
    /* from sample modes.c */
    { VGA_640x480x16, 0xa0000 - FB_BASE, 4, 1, 640, 480,
      { { 0xE3 },
        { 0x03, 0x01, 0x08, 0x00, 0x06 },
        { 0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E, 0x00, 0x40, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0xEA, 0x0C, 0xDF, 0x28, 0x00, 0xE7,
          0x04, 0xE3, 0xFF },
        { 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0F, 0xFF },
        { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07, 0x38, 0x39, 0x3A,
          0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x01, 0x00, 0x0F, 0x00, 0x00 } } },
#endif
#if 0
    /* from sample modes.c */
    { VGA_720x480x16,
      { { 0xE7 },
        { 0x03, 0x01, 0x08, 0x00, 0x06 },
        { 0x6B, 0x59, 0x5A, 0x82, 0x60, 0x8D, 0x0B, 0x3E, 0x00, 0x40, 0x06,
          0x07, 0x00, 0x00, 0x00, 0x00, 0xEA, 0x0C, 0xDF, 0x2D, 0x08, 0xE8,
          0x05, 0xE3, 0xFF },
        { 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0F, 0xFF },
        { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
          0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x01, 0x00, 0x0F, 0x00, 0x00 } } },
#endif
#if 0
    /* from sample modes.c */
    { VGA_320x200x256, 0xa0000 - FB_BASE, 8, 0, 320, 200,
      { { 0x63 },
        { 0x03, 0x01, 0x0F, 0x00, 0x0E },
        { 0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F, 0x00, 0x41, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96,
          0xB9, 0xA3, 0xFF },
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF },
        { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
          0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x41, 0x00, 0x0F, 0x00, 0x00 } } },
#endif
    /* regdump from vga_bios driver */
    { VGA_320x200x256, 0xa0000 - FB_BASE, 8, 0, 320, 200,
      { { 0x63 },
        { 0x03, 0x01, 0x0F, 0x00, 0x0E },
        { 0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F, 0x00, 0x41, 0x00,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x9C, 0x8E, 0x8F, 0x28, 0x40, 0x96,
          0xB9, 0xA3, 0xFF },
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F, 0xFF },
        { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
          0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x41, 0x00, 0x0F, 0x00, 0x00 } } },
    { GFX_MODE_NONE, 0xa0000 - FB_BASE, 0, 0, 0, 0,
      { { 0x67 },
        { 0x03, 0x00, 0x03, 0x00, 0x02 },
        { 0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F, 0x00, 0x4F, 0x0D,
          0x0E, 0x00, 0x00, 0x00, 0x50, 0x9C, 0x0E, 0x8F, 0x28, 0x1F, 0x96,
          0xB9, 0xA3, 0xFF },
        { 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00, 0xFF },
        { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07, 0x38, 0x39, 0x3A,
          0x3B, 0x3C, 0x3D, 0x3E, 0x3F, 0x0C, 0x00, 0x0F, 0x08, 0x00 } } },
};

PRIVATE int init(char *name) {
    int d, r;

    myname = name;

    afb = malloc(0x20000 + PAGE_SIZE);
    if (afb == NULL) panic(myname, "out of memory", errno);

    fb = afb;
    d = ((unsigned)fb % PAGE_SIZE);
    if (d) fb = (unsigned char *) fb + (PAGE_SIZE - d);

    r = sys_vm_map(SELF, 1, (phys_bytes)fb, 0x20000, 0xa0000);
    if (r != OK) panic(myname, "sys_vm_map on 0xa0000 failed", r);

    curfb = fb;

    return 0;
}

#define WRITE_RANGE(num, idx, dta, ary) \
    for (i=0; i<num; i++) { \
        OUTB(idx, i); \
        OUTB(dta, ary[i]); \
    }

PRIVATE void set_plane(int p) {
    unsigned char pmask;

    p &= 3;
    pmask = 1 << p;

    OUTB(VGA_GC_INDEX, 4);
    OUTB(VGA_SEQ_INDEX, 2);
    OUTB(VGA_GC_DATA, p);
    OUTB(VGA_SEQ_DATA, pmask);
}

PRIVATE int set_mode(gfx_mode_t mode) {
    unsigned long lv;
    int x, r, i;
    vga_raw_mode_t *rm;

    for (x=0; mode_list[x].mode != GFX_MODE_NONE; x++) {
        if (mode == mode_list[x].mode) break;
    }
    if (mode_list[x].mode == GFX_MODE_NONE) return EGFX_UNSUPPORTED_MODE;
    rm = &mode_list[x].raw_mode;

    OUTB(VGA_MISC_WRITE, rm->misc[0]);
    WRITE_RANGE(VGA_NUM_SEQ_REGS, VGA_SEQ_INDEX, VGA_SEQ_DATA, rm->seq);
 
    /* unlock CRTC registers and keep them unlocked */
    OUTB(VGA_CRTC_INDEX, 0x03);
    INB(VGA_CRTC_DATA, &lv);
    OUTB(VGA_CRTC_DATA, lv | 0x80);
    OUTB(VGA_CRTC_INDEX, 0x11);
    INB(VGA_CRTC_DATA, &lv);
    OUTB(VGA_CRTC_DATA, lv & ~0x80);
    rm->crtc[0x03] |= 0x80;
    rm->crtc[0x11] &= ~0x80;

    WRITE_RANGE(VGA_NUM_CRTC_REGS, VGA_CRTC_INDEX, VGA_CRTC_DATA, rm->crtc);
    WRITE_RANGE(VGA_NUM_GC_REGS, VGA_GC_INDEX, VGA_GC_DATA, rm->gc);

    for (i=0; i<VGA_NUM_AC_REGS; i++) {
        INB(VGA_INSTAT_READ, &lv);
        OUTB(VGA_AC_INDEX, i);
        OUTB(VGA_AC_WRITE, rm->ac[i]);
    }

    INB(VGA_INSTAT_READ, &lv);
    OUTB(VGA_AC_INDEX, 0x20);

    curfb = &fb[mode_list[x].fb_off];
    width = mode_list[x].width;
    height = mode_list[x].height;
    bpp = mode_list[x].bpp;
    planar = mode_list[x].planar;
    if (bpp == 4) {
        stride = width / 8;
    } else {
        stride = (unsigned int) width * bpp / 8;
    }

    if (planar) set_plane(0);

    return 0;
}

PRIVATE int get_pixel(unsigned short x, unsigned short y, unsigned int *c) {
    return 0;
}

PRIVATE int put_pixel(unsigned short x, unsigned short y, unsigned int c) {

    if (x<0 || x>= width) return EGFX_OUT_OF_RANGE;
    if (y<0 || y>= height) return EGFX_OUT_OF_RANGE;

    if (bpp == 1) { /* 1bpp packed */
        int mask, off;
        c = (!!c) * 0xff;
        off = stride * y + (x >> 3);
        x &= 7;
        mask = 0x80 >> x;
        curfb[off] = (curfb[off] & ~mask) | ( c & mask );
    } else if (bpp == 4) { /* 4bpp is always planar */
        int mask, off;
        off = stride * y + (x >> 3);
        mask = 0x80 >> (x & 7);
        OUTB(VGA_GC_INDEX, 4);
        OUTB(VGA_SEQ_INDEX, 2);

        OUTB(VGA_GC_DATA, 0);
        OUTB(VGA_SEQ_DATA, 0x01);
        curfb[off] = (curfb[off] & ~mask) | ((!!(c&0x01)) * mask);

        OUTB(VGA_GC_DATA, 1);
        OUTB(VGA_SEQ_DATA, 0x02);
        curfb[off] = (curfb[off] & ~mask) | ((!!(c&0x02)) * mask);

        OUTB(VGA_GC_DATA, 2);
        OUTB(VGA_SEQ_DATA, 0x04);
        curfb[off] = (curfb[off] & ~mask) | ((!!(c&0x04)) * mask);

        OUTB(VGA_GC_DATA, 3);
        OUTB(VGA_SEQ_DATA, 0x08);
        curfb[off] = (curfb[off] & ~mask) | ((!!(c&0x08)) * mask);
    } else if (bpp == 8) {
        curfb[stride * y + x] = c;
    }

    return 0;
}

PRIVATE int clear_screen(void) {
    int len, i, p;

    len = width * height / ( planar ? 4 : 1 ) * bpp / 8;
    if (len >= 65536) len = 65536;
    while (len % 4) len++;

    for (p=0; p< (planar ? 4 : 1); p++) {
        if (planar) set_plane(p);
        for (i=0; i<len/4; i++) {
            ((unsigned long *)curfb)[i] = 0;
        }
    }

    return 0;
}

#undef ABS
#define ABS(x) ( ((x)<0) ? (-(x)) : (x) )

PRIVATE int draw_line(unsigned short x1, unsigned short y1,
                      unsigned short x2, unsigned short y2,
                      unsigned int c) {
    int dx, dy, i, e, x, y, t, p;

    dx = x2 - x1;
    dy = y2 - y1;

    c &= 0xff;
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
/* (e>=dx) --> TX=1 else TX=0 */
#define TX (((unsigned int)dx - (unsigned int)e - 1) >> 31)
        if (planar) {
            int mask, off;
            OUTB(VGA_GC_INDEX, 4);
            OUTB(VGA_SEQ_INDEX, 2);
            for (p=0; p<4; p++) {
                OUTB(VGA_GC_DATA, p);
                OUTB(VGA_SEQ_DATA, 0x01 << p);
                e = dx >> 1;
                y = y1;
                for (x=x1; x<=x2; x++) {
                    e = e - TX*dx + dy;
                    y += TX*i;
                    if (x<0 || x>=width) continue;
                    if (y<0 || y>=height) continue;
                    off = stride * y + (x >> 3);
                    mask = 0x80 >> (x & 7);
                    curfb[off] = (curfb[off] & ~mask) | ((!!(c&(0x01<<p))) * mask);
                }
            }
        } else {
            for (x=x1; x<=x2; x++) {
                e = e - TX*dx + dy;
                y += TX*i;
                put_pixel(x, y, c);
            }
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
/* (e>=dx) --> TY=1 else TY=0 */
#define TY (((unsigned int)dy - (unsigned int)e - 1) >> 31)
        if (planar) {
            int mask, off;
            OUTB(VGA_GC_INDEX, 4);
            OUTB(VGA_SEQ_INDEX, 2);
            for (p=0; p<4; p++) {
                OUTB(VGA_GC_DATA, p);
                OUTB(VGA_SEQ_DATA, 0x01 << p);
                e = dy >> 1;
                x = x1;
                for (y=y1; y<=y2; y++) {
                    e = e - TY*dy + dx;
                    x += TY*i;
                    if (x<0 || x>=width) continue;
                    if (y<0 || y>=height) continue;
                    off = stride * y + (x >> 3);
                    mask = 0x80 >> (x & 7);
                    curfb[off] = (curfb[off] & ~mask) | ((!!(c&(0x01<<p))) * mask);
                }
            }
        } else {
            for (y=y1; y<=y2; y++) {
                e = e - TY*dy + dx;
                x += TY*i;
                put_pixel(x, y, c);
            }
        }
    }

    return 0;
}

PRIVATE int draw_line_hori(unsigned short x1, unsigned short y1,
                           unsigned short x2,
                           unsigned int c) {
    int tx;

    if (y1<0 || y1>=height) return EGFX_OUT_OF_RANGE;
    if (x1>x2) {
        tx=x1; x1=x2; x2=tx;
    }
    if (x1 >= width && x2 >= width) return EGFX_OUT_OF_RANGE;
    if (x1<0 && x2<0) return EGFX_OUT_OF_RANGE;
    if (x1 < 0) x1 = 0;
    if (x2 >= width) x2 = width-1;

    if (planar) {
        int x, mask, off, p;
        c &= 0x0f;
        OUTB(VGA_GC_INDEX, 4);
        OUTB(VGA_SEQ_INDEX, 2);
        for (p=0; p<4; p++) {
            OUTB(VGA_GC_DATA, p);
            OUTB(VGA_SEQ_DATA, 0x01 << p);
            for (x=x1; x<=x2; x++) {
                off = stride * y1 + (x >> 3);
                mask = 0x80 >> (x & 7);
                curfb[off] = (curfb[off] & ~mask) | ((!!(c&(0x01<<p))) * mask);
            }
        }
    } else {
        for (; x1<=x2; x1++) put_pixel(x1, y1, c);
    }

    return 0;
}

PRIVATE int draw_line_vert(unsigned short x1, unsigned short y1,
                                              unsigned short y2,
                           unsigned int c) {
    int ty;

    if (x1<0 || x1>=width) return EGFX_OUT_OF_RANGE;
    if (y1>y2) {
        ty=y1; y1=y2; y2=ty;
    }
    if (y1 >= height && y2 >= height) return EGFX_OUT_OF_RANGE;
    if (y1<0 && y2<0) return EGFX_OUT_OF_RANGE;
    if (y1 < 0) y1 = 0;
    if (y2 >= height) y2 = height-1;

    if (planar) {
        int y, mask, off, p;
        c &= 0x0f;
        OUTB(VGA_GC_INDEX, 4);
        OUTB(VGA_SEQ_INDEX, 2);
        for (p=0; p<4; p++) {
            OUTB(VGA_GC_DATA, p);
            OUTB(VGA_SEQ_DATA, 0x01 << p);
            for (y=y1; y<=y2; y++) {
                off = stride * y + (x1 >> 3);
                mask = 0x80 >> (x1 & 7);
                curfb[off] = (curfb[off] & ~mask) | ((!!(c&(0x01<<p))) * mask);
            }
        }
    } else {
        for (; y1<=y2; y1++) put_pixel(x1, y1, c);
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

PUBLIC gfx_funcs_t gfx_funcs_vga_raw = {
    "vga_raw",
    TEXT_COLOR | VGA_640x480x2 | VGA_640x480x16 | VGA_320x200x256,
    init,
    set_mode,
    get_pixel,
    put_pixel,
    clear_screen,
    draw_line,
    draw_line_hori,
    draw_line_vert,
    generic_draw_rect,
    generic_put_char
};
