
#define _POSIX_SOURCE
#define _MINIX
#define _SYSTEM

#include <minix/config.h>
#include <minix/com.h>
#include <minix/type.h>
#include <minix/syslib.h>
#include <minix/sysutil.h>
#include <minix/const.h>
#include <minix/callnr.h>
#include <ibm/int86.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#include "gfx.h"
#include "gfx_ioctl.h"
#include "vga_regs.h"

#define MYNAME "gfx_ioctl"

EXTERN int debug;
EXTERN gfx_funcs_t *driver;

PRIVATE gfx_request_set_mode_t mode;
PRIVATE gfx_request_pixel_t pixel;
PRIVATE gfx_request_line_t line;
PRIVATE gfx_request_rect_t rect;

FORWARD int dump_registers(vga_registers_t *regs);

PUBLIC int gfx_ioctl(message *mess) {
    int r;

    switch (mess->REQUEST) {
        case GFX_REQUEST_SET_MODE:
            r = sys_vircopy(mess->IO_ENDPT, D, (vir_bytes) mess->ADDRESS,
                            SELF,           D, (vir_bytes) &mode,
                            sizeof(mode));
            if (r != OK) return EGFX_ERROR;
            DEBUG report(MYNAME, "gfx_request_set_mode", mode);
            if ((driver->modes & mode) == 0) return EGFX_UNSUPPORTED_MODE;
            return driver->set_mode(mode);
            break;
        case GFX_REQUEST_GET_PIXEL:
            r = sys_vircopy(mess->IO_ENDPT, D, (vir_bytes) mess->ADDRESS,
                            SELF,           D, (vir_bytes) &pixel,
                            sizeof(pixel));
            if (r != OK) return EGFX_ERROR;
            driver->get_pixel(pixel.x, pixel.y, &pixel.c);
            r = sys_vircopy(SELF,           D, (vir_bytes) &pixel,
                            mess->IO_ENDPT, D, (vir_bytes) mess->ADDRESS,
                            sizeof(pixel));
            if (r != OK) return EGFX_ERROR;
            return 0;
            break;
        case GFX_REQUEST_PUT_PIXEL:
            r = sys_vircopy(mess->IO_ENDPT, D, (vir_bytes) mess->ADDRESS,
                            SELF,           D, (vir_bytes) &pixel,
                            sizeof(pixel));
            if (r != OK) return EGFX_ERROR;
            return driver->put_pixel(pixel.x, pixel.y, pixel.c);
            break;
        case GFX_REQUEST_DRAW_LINE:
            r = sys_vircopy(mess->IO_ENDPT, D, (vir_bytes) mess->ADDRESS,
                            SELF,           D, (vir_bytes) &line,
                            sizeof(line));
            if (r != OK) return EGFX_ERROR;
            return driver->draw_line(line.x1, line.y1, line.x2, line.y2, line.c);
            break;
        case GFX_REQUEST_DRAW_LINE_HORI:
            r = sys_vircopy(mess->IO_ENDPT, D, (vir_bytes) mess->ADDRESS,
                            SELF,           D, (vir_bytes) &line,
                            sizeof(line));
            if (r != OK) return EGFX_ERROR;
            return driver->draw_line_hori(line.x1, line.y1, line.x2, line.c);
            break;
        case GFX_REQUEST_DRAW_LINE_VERT:
            r = sys_vircopy(mess->IO_ENDPT, D, (vir_bytes) mess->ADDRESS,
                            SELF,           D, (vir_bytes) &line,
                            sizeof(line));
            if (r != OK) return EGFX_ERROR;
            return driver->draw_line_vert(line.x1, line.y1, line.y2, line.c);
            break;
        case GFX_REQUEST_CLEAR_SCREEN:
            return driver->clear_screen();
        case GFX_REQUEST_DUMP_REGISTERS: {
            vga_registers_t regs;

            dump_registers(&regs);
            r = sys_vircopy(SELF,           D, (vir_bytes) &regs,
                            mess->IO_ENDPT, D, (vir_bytes) mess->ADDRESS,
                            sizeof(regs));
            if (r != OK) return EGFX_ERROR;
            return 0;
            break;
        }
        case GFX_REQUEST_RESET: {
            struct reg86u reg86;

            reg86.u.b.intno = 0x10;
            reg86.u.w.ax = 0x0003;
            r = sys_int86(&reg86);
            if (r != OK) return EGFX_ERROR;
            return 0;
            break;
        }
        case GFX_REQUEST_DRAW_RECT:
            r = sys_vircopy(mess->IO_ENDPT, D, (vir_bytes) mess->ADDRESS,
                            SELF,           D, (vir_bytes) &rect,
                            sizeof(rect));
            if (r != OK) return EGFX_ERROR;
            return driver->draw_rect(rect.x1, rect.y1, rect.x2, rect.y2, rect.c);
            break;
        default:
            break;
    }

    return 0;
}

#define READ_RANGE(num, idx, dta, ary) \
    for (i=0; i<num; i++) { \
        sys_outb(idx, i); \
        sys_inb(dta, &lv); \
        regs->ary[i] = lv; \
    }

PRIVATE int dump_registers(vga_registers_t *regs) {
    unsigned long lv, i;

    sys_inb(VGA_MISC_READ, &lv);
    regs->misc[0] = lv;

    READ_RANGE(VGA_NUM_SEQ_REGS,  VGA_SEQ_INDEX,  VGA_SEQ_DATA,  seq);
    READ_RANGE(VGA_NUM_CRTC_REGS, VGA_CRTC_INDEX, VGA_CRTC_DATA, crtc);
    READ_RANGE(VGA_NUM_GC_REGS,   VGA_GC_INDEX,   VGA_GC_DATA,   gc);
    for (i=0; i<VGA_NUM_AC_REGS; i++) {
        sys_inb(VGA_INSTAT_READ, &lv);
        sys_outb(VGA_AC_INDEX, i);
        sys_inb(VGA_AC_READ, &lv);
        regs->ac[i] = lv;
    }

    return 0;
}
