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
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
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

    current_mode = GFX_MODE_NONE;
    return 0;
}

PRIVATE int set_mode(gfx_mode_t mode) {
    return 0;
}

PRIVATE int put_pixel(unsigned short x, unsigned short y, unsigned int c) {
    return 0;
}

PRIVATE int clear_screen(void) {
    return 0;
}

PRIVATE int draw_line_hori(unsigned short x1, unsigned short y1,
                           unsigned short x2,
                           unsigned int c) {
    return 0;
}

PRIVATE int draw_line_vert(unsigned short x1, unsigned short y1,
                                              unsigned short y2,
                           unsigned int c) {
    return 0;
}

PRIVATE int draw_line(unsigned short x1, unsigned short y1,
                      unsigned short x2, unsigned short y2,
                      unsigned int c) {
    return 0;
}

PRIVATE int draw_rect(unsigned short x1, unsigned short y1,
                      unsigned short x2, unsigned short y2,
                      unsigned int c) {
    return -1;
}

PRIVATE int put_char(unsigned short x, unsigned short y,
                     unsigned int c, unsigned char chr, gfx_font_t f) {
    return -1;
}

PRIVATE int put_string(unsigned short x, unsigned short y,
                       unsigned int c, unsigned char *str,
                       unsigned int len, gfx_font_t f) {
    return -1;
}

PUBLIC gfx_funcs_t gfx_funcs_svga_vesa = {
    "svga_vesa",
    GFX_MODE_NONE,
    init,
    set_mode,
    put_pixel,
    clear_screen,
    draw_line,
    draw_line_hori,
    draw_line_vert,
    draw_rect,
    put_char,
    put_string
};
