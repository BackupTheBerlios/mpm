/*
 * mem_minix.c - get memory/swap usages in percent, for Minix
 * copyright (c) 2006 by Ivo van Poorten
 *
 * licensed under the GPL version 2
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <minix/config.h>
#include <minix/const.h>
#include <minix/com.h>
#include <sys/svrctl.h>

static unsigned long tm = 1;	/* total memory */

void mem_init(void) {
    struct sysgetenv sge;
    char v[1024];

    sge.key="memory";
    sge.keylen=7;
    sge.val=v;
    sge.vallen=sizeof(v);

    if (svrctl(MMGETPARAM, &sge) < 0 || sscanf(v, "%*x:%*x,%*x:%x", &tm) != 1)
        fprintf(stderr, "could not determine total memory size\n");
    tm /= 1024;	/* store in kilobytes */
}

void mem_getusage(int *per_mem, int *per_swap, const struct mem_options *opts) {
    struct pm_mem_info pmi;
    int h;
    unsigned long tf;	/* total free memory */

    if (getsysinfo(PM_PROC_NR, SI_MEM_ALLOC, &pmi) < 0) {
        fprintf(stderr, "cannot retrieve kernel mem alloc structure\n");
        return;
    }

    for (h=tf=0; h<_NR_HOLES; h++)
        tf+= pmi.pmi_holes[h].h_base? pmi.pmi_holes[h].h_len << CLICK_SHIFT : 0;

    tf /= 1024;
    *per_mem=(tm-tf) * 100 / tm;
    *per_swap=0;
}
