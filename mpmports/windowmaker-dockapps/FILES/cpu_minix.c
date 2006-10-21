/*
 * cpu_minix - get cpu usage, for Minix 
 * copyright (c) 2006 by Ivo van Poorten
 *
 * Licensed under the GPL version 2
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <timers.h>
#include <minix/config.h>
#include <minix/type.h>
#include <minix/const.h>
#include <minix/ipc.h>
#include <sys/times.h>
#include "/usr/src/kernel/const.h"
#include "/usr/src/kernel/proc.h"

#include "cpu.h"

#define PROCS (NR_TASKS + NR_PROCS)

struct proc prevproc[PROCS], proc[PROCS];
static int uptime, prevuptime;

void cpu_init(void) {
	struct tms tms;
	if (getsysinfo(PM_PROC_NR, SI_KPROC_TAB, prevproc) < 0) {
		fprintf(stderr, "cannot retrieve kproc table\n");
	}
	prevuptime = times(&tms);
}

int cpu_get_usage(cpu_options *opts) {
	struct tms tms;
	int dt, p, idletime;

	uptime = times(&tms);
	dt = uptime - prevuptime;

	if (getsysinfo(PM_PROC_NR, SI_KPROC_TAB, proc) < 0) {
		fprintf(stderr, "cannot retrieve kproc table\n");
		return 0;
	}

	idletime = proc[0].p_user_time - prevproc[0].p_user_time;

	/* only copy IDLE task, which is the first task in the list */
	memcpy(prevproc, proc, sizeof(struct proc));
	prevuptime = uptime;
	return 100 - 100 * idletime / dt;
}
