/*
 * uptime_minix.c - get uptime in seconds for Minix
 * copyright (c) 2006 by Ivo van Poorten
 *
 * licensed under the GPL version 2
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <utmp.h>
#include <time.h>
#include <minix/paths.h>

int get_uptime()
{
    static time_t boot_time = -1;

    if (boot_time == -1) {
        FILE *f;
        long s;
        struct utmp wtmp_entry;

        if ( (f = fopen(_PATH_WTMP, "r")) == NULL) goto errout;
        if (fseek(f, 0L, 2) !=0 ) goto errout;
        if ((s = ftell(f)) % sizeof(struct utmp) != 0) goto errout;
        if ((s /= sizeof(struct utmp)) == 0) goto errout;

        while (s>0) {
            s--;
            fseek(f, s*sizeof(struct utmp), 0);
            if (fread(&wtmp_entry, sizeof(struct utmp), 1, f) != 1) goto errout;
            if (wtmp_entry.ut_type == BOOT_TIME) break;
        }
errout:
        boot_time = wtmp_entry.ut_type == BOOT_TIME ? wtmp_entry.ut_time : 0;
        fclose(f);
    }

    return time(NULL) - boot_time;
}

