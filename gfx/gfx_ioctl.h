#ifndef GFX_IOC_H
#define GFX_IOC_H

_PROTOTYPE(int gfx_ioctl, (message *mess));

#define GFX_REQUEST_BASE            0x12340000

#define GFX_REQUEST_SET_MODE        (GFX_REQUEST_BASE + 0x01)
#define GFX_REQUEST_GET_PIXEL       (GFX_REQUEST_BASE + 0x02)
#define GFX_REQUEST_PUT_PIXEL       (GFX_REQUEST_BASE + 0x03)
#define GFX_REQUEST_CLEAR_SCREEN    (GFX_REQUEST_BASE + 0x04)

typedef unsigned long gfx_request_set_mode_t;
typedef struct gfx_request_pixel_s {
    int x, y, c;
} gfx_request_pixel_t;

#define GFX_OK                      0

#define EGFX_BASE                   GFX_REQUEST_BASE

#define EGFX_ERROR                  _SIGN (EGFX_BASE)
#define EGFX_UNSUPPORTED_MODE       _SIGN (EGFX_BASE + 0x01)
#define EGFX_OUT_OF_RANGE           _SIGN (EGFX_BASE + 0x02)

#endif
