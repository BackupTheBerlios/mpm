#ifndef GFX_GENERIC_H
#define GFX_GENERIC_H

EXTERN int generic_draw_rect(unsigned short x1, unsigned short y1,
                             unsigned short x2, unsigned short y2,
                             unsigned int c);
EXTERN int generic_put_char(unsigned short x, unsigned short y,
                            unsigned int c, unsigned char chr, gfx_font_t f);
EXTERN int generic_put_string(unsigned short x, unsigned short y,
                              unsigned int c, unsigned char *s,
                              unsigned int len, gfx_font_t f);

#endif
