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

#define MYPIDFILE "/usr/run/gfx.pid"
#define MYNAME "gfx"

EXTERN gfx_funcs_t gfx_funcs_vga_bios;
EXTERN gfx_funcs_t gfx_funcs_vga_raw;
EXTERN gfx_funcs_t gfx_funcs_svga_vesa;

PUBLIC int debug = 0;
PUBLIC gfx_funcs_t *driver = &gfx_funcs_vga_bios;

PRIVATE struct driverlist_s {
    char *name;
    gfx_funcs_t *driver;
} drivers[] = {
    { "vga_bios", &gfx_funcs_vga_bios },
    { "vga_raw",  &gfx_funcs_vga_raw  },
    { "svga_vesa",  &gfx_funcs_svga_vesa  },
    { NULL, NULL }
};

PRIVATE void write_pidfile(void) {
    FILE *fp;

    fp = fopen(MYPIDFILE, "w");
    fprintf(fp, "%d\n", getpid());
    fclose(fp);
}

PRIVATE void parse_args(int argc, char **argv) {
    int x, y;

    for (x=1; x<argc; x++) {
        if (!strcmp(argv[x], "debug")) {
            debug = 1;
            continue;
        } else if (!strncmp(argv[x], "driver=", 7)) {
            for (y=0; drivers[y].name; y++) {
                if (!strcmp(&argv[x][7], drivers[y].name)) {
                    driver = drivers[y].driver;
                    break;
                }
            }
        }
    }
}

PUBLIC int main(int argc, char **argv) {
    message mess;
    int caller, proc_nr, r;

    write_pidfile();
    parse_args(argc, argv);

    driver->init(driver->name);

    while(1) {
        if (receive(ANY, &mess) != OK) continue;

        caller = mess.m_source;
        proc_nr = mess.IO_ENDPT;

        DEBUG report(MYNAME, "mess.m_type", mess.m_type);

        r = 0;
        switch (mess.m_type) {
            case DEV_IOCTL:     r = gfx_ioctl(&mess);       break;
            case HARD_INT:
            case SYS_SIG:
            case SYN_ALARM:
            case DEV_PING:
            case PROC_EVENT:
                continue;
            default:
                break;
        }

        DEBUG report(MYNAME, "sending reply, r = ", r);

        mess.m_type = TASK_REPLY;
        mess.REP_ENDPT = proc_nr;
        mess.REP_STATUS = r;
        send(caller, &mess);
    }

    return 0;
}
