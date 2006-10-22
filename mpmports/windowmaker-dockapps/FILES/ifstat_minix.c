/*
 * copyright (c) 2006 by Ivo van Poorten
 *
 * Minix-specific interface statistics gathering routines.
 */

#include <config.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/gen/ether.h>
#include <net/gen/eth_io.h>

#include "ifstat.h"
#include "utils.h"

/* Average packet sizes for upstream and downstream
 *
 * Received packets are significantly larger on average than transmitted
 * packets. I cannot find a decent study though, so I just pick some
 * numbers that seem to look pretty ok (with a margin of 25-50% ;-) )
 *
 * If anybody knows a better and more accurate way of determining the
 * amount of data passed through an interface, let me know and/or send
 * a patch :)
 */

#define AVGUPSIZE   150
#define AVGDOWNSIZE 250

struct ifstatstate {
    int dummy;
};

ifstatstate_t * if_statinit(void) {
	ifstatstate_t	*statep;

	statep = calloc(1, sizeof (ifstatstate_t));
	if (statep == NULL) {
		warn("cannot allocate interface statistics state");
		return (NULL);
	}

	return (statep);
}

int if_stats(const char *ifname, ifstatstate_t *statep, ifstats_t *ifstatsp) {
	char devname[256];
    int fd;
    struct nwio_ethstat ethstat;
    unsigned long pktsR, pktsT;

    snprintf(devname, 256, "/dev/%s", ifname);

    fd = open(devname, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "cannot open %s\n", devname);
        return 0;
    }

    ioctl(fd, NWIOGETHSTAT, &ethstat);
    close(fd);

    pktsR = ethstat.nwes_stat.ets_packetR;
    pktsT = ethstat.nwes_stat.ets_packetT;

	ifstatsp->rxbytes = pktsR * AVGDOWNSIZE;
	ifstatsp->txbytes = pktsT * AVGUPSIZE;

	return 1;
}

void if_statfini(ifstatstate_t *statep) {
	free(statep);
}
