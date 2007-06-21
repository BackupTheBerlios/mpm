/*
 * Copyright (c) 2006, Ivo van Poorten
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * My name may NOT be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER(S) ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#define _POSIX_SOURCE
#define _MINIX
#define _SYSTEM

#include "config.h"

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
#include <stdlib.h>

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
PRIVATE gfx_request_char_t chr;
PRIVATE gfx_request_string_t string;

FORWARD int dump_registers(vga_registers_t *regs);

PUBLIC int gfx_ioctl(message *mess) {
    int r;

    switch (mess->REQUEST) {
        case GFX_REQUEST_SET_MODE:
            r = sys_vircopy(mess->IO_ENDPT, D, (vir_bytes) mess->ADDRESS,
                            SELF,           D, (vir_bytes) &mode,
                            sizeof(mode));
            if (r != OK) return EGFX_ERROR;
            if ((driver->modes & mode) == 0) return EGFX_UNSUPPORTED_MODE;
            return driver->set_mode(mode);
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
        case GFX_REQUEST_PUT_CHAR:
            r = sys_vircopy(mess->IO_ENDPT, D, (vir_bytes) mess->ADDRESS,
                            SELF,           D, (vir_bytes) &chr,
                            sizeof(chr));
            if (r != OK) return EGFX_ERROR;
            return driver->put_char(chr.x, chr.y, chr.c, chr.chr, chr.f);
            break;
        case GFX_REQUEST_PUT_STRING: {
            unsigned char *s;
            r = sys_vircopy(mess->IO_ENDPT, D, (vir_bytes) mess->ADDRESS,
                            SELF,           D, (vir_bytes) &string,
                            sizeof(string));
            if (r != OK) return EGFX_ERROR;
            s = malloc(string.len);
            if (!s) return EGFX_OUT_OF_MEMORY;
            r = sys_vircopy(mess->IO_ENDPT, D, (vir_bytes) string.s,
                            SELF,           D, (vir_bytes) s,
                            string.len);
            if (r != OK) {
                free(s);
                return EGFX_ERROR;
            }
            r = driver->put_string(string.x, string.y, string.c, s, string.len,
                                   string.f);
            free(s);
            return r;
            break;
        }
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
