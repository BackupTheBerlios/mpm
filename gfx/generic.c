#define _POSIX_SOURCE
#define _MINIX
#define _SYSTEM

#include <minix/config.h>
#include <minix/com.h>
#include <minix/type.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/const.h>
#include <errno.h>

#include "gfx.h"
#include "gfx_ioctl.h"
#include "fonts.h"

EXTERN gfx_funcs_t *driver;

PUBLIC int generic_draw_rect(unsigned short x1, unsigned short y1,
                             unsigned short x2, unsigned short y2,
                             unsigned int c) {
    int r;

    r  = driver->draw_line_hori(x1, y1, x2, c);
    r += driver->draw_line_hori(x1, y2, x2, c);
    r += driver->draw_line_vert(x1, y1, y2, c);
    r += driver->draw_line_vert(x2, y1, y2, c);

    if (r<0) r = EGFX_OUT_OF_RANGE;
    return r;
}

PUBLIC int generic_put_char(unsigned short x, unsigned short y,
                            unsigned int c, unsigned char chr, gfx_font_t f) {
    int h, cnt, p;
    unsigned char *bm;

    switch (f) {
        case GFX_FONT_8x8:
            h = 8;
            bm = &font_8x8[chr*h];
            break;
        case GFX_FONT_8x16:
            h = 16;
            bm = &font_8x16[chr*h];
            break;
        default:
            return EGFX_ERROR;
    }

    for (cnt=0; cnt<h; cnt++) {
        for (p=0; p<8; p++) {
            if (bm[cnt]&(0x80>>p)) {
                driver->put_pixel(x+p, y+cnt, c);
            }
        }
    }

    return 0;
}

PUBLIC int generic_put_string(unsigned short x, unsigned short y,
                              unsigned int c, unsigned char *s,
                              unsigned int len, gfx_font_t f) {
    int i;

    for (i=0; i<len; i++) {
        driver->put_char(x+i*8, y, c, s[i], f);
    }
    return 0;
}

