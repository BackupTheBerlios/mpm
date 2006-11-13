
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

#define MYNAME "gfx_ioctl"

EXTERN int debug;
EXTERN gfx_funcs_t *driver;

PRIVATE gfx_request_set_mode_t mode;
PRIVATE gfx_request_pixel_t pixel;
PRIVATE gfx_request_line_t line;

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
        default:
            break;
    }

    return 0;
}
