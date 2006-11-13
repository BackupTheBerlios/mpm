#ifndef VGA_H
#define VGA_H

#define DEBUG if (debug)

typedef enum gfx_mode_e {
    GFX_MODE_NONE   = 0x00000000,
    TEXT_MONO       = 0x00000001,
    TEXT_COLOR      = 0x00000002,
    EGA_320x200x16  = 0x00000004,
    EGA_640x350x16  = 0x00000008,
    VGA_640x480x16  = 0x00000010,
    VGA_320x200x256 = 0x00000020
} gfx_mode_t;

typedef struct gfx_funcs_s {
    char *name;
    unsigned long modes;
    int (*init)(char *name);
    int (*set_mode)(gfx_mode_t mode);
    int (*get_pixel)(int x, int y, int *c);
    int (*put_pixel)(int x, int y, int c);
    int (*clear_screen)(void);
    int (*draw_line)(int x1, int y1, int x2, int y2, int c);
    int (*draw_line_hori)(int x1, int y1, int x2, int c);
    int (*draw_line_vert)(int x1, int y1, int y2, int c);
} gfx_funcs_t;

#endif
