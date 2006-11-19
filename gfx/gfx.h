#ifndef GFX_H
#define GFX_H

#define DEBUG if (debug)

typedef enum gfx_mode_e {
    GFX_MODE_NONE   = 0x00000000,
    TEXT_MONO       = 0x00000001,
    TEXT_COLOR      = 0x00000002,
    VGA_640x480x2   = 0x00000004,
    VGA_640x480x16  = 0x00000008,
    VGA_320x200x256 = 0x00000010
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
