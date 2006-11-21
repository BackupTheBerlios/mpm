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

#ifndef GFX_H
#define GFX_H

#define DEBUG if (debug)

typedef enum gfx_mode_e {
    GFX_MODE_NONE       = 0x00000000,
    VGA_640x480x2       = 0x00000001,
    VGA_640x480x16      = 0x00000002,
    VGA_320x200x256     = 0x00000004,
    SVGA_640x400x256    = 0x00000008,
    SVGA_640x480x256    = 0x00000010,
    SVGA_640x480x32k    = 0x00000020,
    SVGA_640x480x64k    = 0x00000040,
    SVGA_640x480x16M    = 0x00000080,
    SVGA_800x600x256    = 0x00000100,
    SVGA_800x600x32k    = 0x00000200,
    SVGA_800x600x64k    = 0x00000400,
    SVGA_800x600x16M    = 0x00000800,
    SVGA_1024x768x256   = 0x00001000,
    SVGA_1024x768x32k   = 0x00002000,
    SVGA_1024x768x64k   = 0x00004000,
    SVGA_1024x768x16M   = 0x00008000,
    SVGA_1280x1024x256  = 0x00010000,
    SVGA_1280x1024x32k  = 0x00020000,
    SVGA_1280x1024x64k  = 0x00040000,
    SVGA_1280x1024x16M  = 0x00080000,
} gfx_mode_t;

typedef enum gfx_font_e {
    GFX_FONT_8x8    = 0x00,
    GFX_FONT_8x16   = 0x01
} gfx_font_t;

typedef struct gfx_funcs_s {
    char *name;
    unsigned long modes;
    int (*init)(char *name);
    int (*set_mode)(gfx_mode_t mode);
    int (*put_pixel)(unsigned short x, unsigned short y, unsigned int c);
    int (*clear_screen)(void);
    int (*draw_line)(unsigned short x1, unsigned short y1,
                     unsigned short x2, unsigned short y2,
                     unsigned int c);
    int (*draw_line_hori)(unsigned short x1, unsigned short y1,
                          unsigned short x2,
                          unsigned int c);
    int (*draw_line_vert)(unsigned short x1, unsigned short y1,
                                             unsigned short y2,
                          unsigned int c);
    int (*draw_rect)(unsigned short x1, unsigned short y1,
                     unsigned short x2, unsigned short y2,
                     unsigned int c);
    int (*put_char)(unsigned short x, unsigned short y,
                    unsigned int c, unsigned char chr, gfx_font_t f);
    int (*put_string)(unsigned short x, unsigned short y,
                      unsigned int c, unsigned char *s,
                      unsigned int len, gfx_font_t f);
} gfx_funcs_t;

#ifdef GFX_DRIVER
#define _PROTOPRIV(a,b)     PRIVATE _PROTOTYPE(a,b)

_PROTOPRIV( int init,           (char *name)                            );
_PROTOPRIV( int set_mode,       (gfx_mode_t mode)                       );
_PROTOPRIV( int put_pixel,      (unsigned short x, unsigned short y,
                                 unsigned int c)                        );
_PROTOPRIV( int clear_screen,   (void)                                  );
_PROTOPRIV( int draw_line,      (unsigned short x1, unsigned short y1,
                                 unsigned short x2, unsigned short y2,
                                 unsigned int c)                        );
_PROTOPRIV( int draw_line_hori, (unsigned short x1, unsigned short y1,
                                 unsigned short x2,
                                 unsigned int c)                        );
_PROTOPRIV( int draw_line_vert, (unsigned short x1, unsigned short y1,
                                                    unsigned short y2,
                                 unsigned int c)                        );
_PROTOPRIV( int draw_rect,      (unsigned short x1, unsigned short y1,
                                 unsigned short x2, unsigned short y2,
                                 unsigned int c)                        );
_PROTOPRIV( int put_char,       (unsigned short x, unsigned short y,
                                 unsigned int c, unsigned char chr,
                                 gfx_font_t f)                          );
_PROTOPRIV( int put_string,     (unsigned short x, unsigned short y,
                                 unsigned int c, unsigned char *s,
                                 unsigned int len, gfx_font_t f)        );
#endif

#endif
