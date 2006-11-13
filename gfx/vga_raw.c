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

#include "gfx.h"

EXTERN int debug;

PRIVATE char *myname;

PRIVATE int init(char *name) {

    myname = name;
    return 0;
}

PRIVATE int set_mode(gfx_mode_t mode) {
    return 0;
}

PRIVATE int get_pixel(int x, int y, int *c) {
    return 0;
}

PRIVATE int put_pixel(int x, int y, int c) {
    return 0;
}

PRIVATE int clear_screen(void) {
    return 0;
}

PRIVATE int draw_line(int x1, int y1, int x2, int y2, int c) {
    return 0;
}

PRIVATE int draw_line_hori(int x1, int y1, int x2, int c) {
    return 0;
}

PRIVATE int draw_line_vert(int x1, int y1, int y2, int c) {
    return 0;
}

PUBLIC gfx_funcs_t gfx_funcs_vga_raw = {
    "vga_raw",
    TEXT_MONO | TEXT_COLOR | EGA_320x200x16 | EGA_640x350x16 | VGA_640x480x16 |
    VGA_320x200x256,
    init,
    set_mode,
    get_pixel,
    put_pixel,
    clear_screen,
    draw_line,
    draw_line_hori,
    draw_line_vert
};
