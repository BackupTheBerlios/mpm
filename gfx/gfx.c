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

EXTERN gfx_funcs_t gfx_funcs_ega_bios;
EXTERN gfx_funcs_t gfx_funcs_vga_bios;
EXTERN gfx_funcs_t gfx_funcs_vga_raw;

PUBLIC int debug = 0;
PUBLIC gfx_funcs_t *driver = &gfx_funcs_vga_bios;

PRIVATE struct driverlist_s {
    char *name;
    gfx_funcs_t *driver;
} drivers[] = {
    { "ega_bios", &gfx_funcs_ega_bios },
    { "vga_bios", &gfx_funcs_vga_bios },
    { "vga_raw",  &gfx_funcs_vga_raw  },
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

        report(MYNAME, "mess.m_type", mess.m_type);

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
