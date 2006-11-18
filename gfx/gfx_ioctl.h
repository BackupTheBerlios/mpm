#ifndef GFX_IOC_H
#define GFX_IOC_H

_PROTOTYPE(int gfx_ioctl, (message *mess));

#define GFX_REQUEST_BASE            0x12340000

#define GFX_REQUEST_SET_MODE        (GFX_REQUEST_BASE + 0x01)
#define GFX_REQUEST_GET_PIXEL       (GFX_REQUEST_BASE + 0x02)
#define GFX_REQUEST_PUT_PIXEL       (GFX_REQUEST_BASE + 0x03)
#define GFX_REQUEST_CLEAR_SCREEN    (GFX_REQUEST_BASE + 0x04)
#define GFX_REQUEST_DRAW_LINE       (GFX_REQUEST_BASE + 0x05)
#define GFX_REQUEST_DRAW_LINE_HORI  (GFX_REQUEST_BASE + 0x06)
#define GFX_REQUEST_DRAW_LINE_VERT  (GFX_REQUEST_BASE + 0x07)
#define GFX_REQUEST_DUMP_REGISTERS  (GFX_REQUEST_BASE + 0x08)
#define GFX_REQUEST_RESET           (GFX_REQUEST_BASE + 0x09)
#define GFX_REQUEST_DRAW_RECT       (GFX_REQUEST_BASE + 0x0a)
#define GFX_REQUEST_PUT_CHAR        (GFX_REQUEST_BASE + 0x0b)

typedef unsigned long gfx_request_set_mode_t;

typedef struct gfx_request_pixel_s {
    unsigned short x, y;
    unsigned int c;
} gfx_request_pixel_t;

typedef struct gfx_request_line_s {
    unsigned short x1, y1, x2, y2;
    unsigned int c;
} gfx_request_line_t;

typedef gfx_request_line_t gfx_request_rect_t;

typedef struct gfx_request_char_s {
    unsigned short x, y;
    unsigned int c;
    unsigned char chr;
    unsigned char f;
} gfx_request_char_t;

#define GFX_OK                      0

#define EGFX_BASE                   GFX_REQUEST_BASE

#define EGFX_ERROR                  _SIGN (EGFX_BASE)
#define EGFX_UNSUPPORTED_MODE       _SIGN (EGFX_BASE + 0x01)
#define EGFX_OUT_OF_RANGE           _SIGN (EGFX_BASE + 0x02)

#endif
