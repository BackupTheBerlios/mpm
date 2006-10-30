/* ------------------------------------------------------------------------- */

/*
 * bzcp <infile.bz2> <outfile>
 * Copyright (C) 2006 Ivo van Poorten. All rights reserved.
 * See LICENSE.bzcp for details
 *
 * bzlib.a
 * Copyright (C) 1996-2005 Julian R Seward. All rights reserved.
 * See LICENSE.bzlib for details
 *
 */

/* ------------------------------------------------------------------------- */

#include <unistd.h>
#include <fcntl.h>
#include "bzlib.h"

/* ------------------------------------------------------------------------- */

#define BUFSIZE 1024

/* ------------------------------------------------------------------------- */

static char inbuf[BUFSIZE], outbuf[BUFSIZE];
static bz_stream strm;

/* ------------------------------------------------------------------------- */

void bz_internal_error(int errcode) {
    write(2, "bzcp: bzlib: bz_internal_error\n", 31);
    _exit(3);
}

/* ------------------------------------------------------------------------- */

int bzcp(char *infile, char *outfile) {
    int fdin, fdout, sin, ret;

    if ( BZ2_bzDecompressInit(&strm, 0, 0) < 0 ) {
        write(2, "bzcp: cannot initialize bzlib\n", 30);
        goto errout;
    }
    if ( (fdin = open(infile, O_RDONLY)) < 0 ) {
        write(2, "bzcp: cannot open input\n", 24);
        goto errout;
    }
    if ( (fdout = open(outfile, O_WRONLY | O_CREAT)) < 0 ) {
        write(2, "bzcp: cannot open output\n", 25);
        goto errout;
    }

    sin = read(fdin, inbuf, BUFSIZE);
    while(sin) {
        strm.next_in = inbuf;
        strm.avail_in = sin;
        do {
            strm.next_out = outbuf;
            strm.avail_out = BUFSIZE;
            ret = BZ2_bzDecompress(&strm);
            if (ret < 0) {
                write(2, "bzcp: error during decompression\n", 33);
                goto errout;
            }
            write(fdout, outbuf, BUFSIZE - strm.avail_out);
        } while (strm.avail_in);
        sin = read(fdin, inbuf, BUFSIZE);
    }
    while (ret >= 0 && ret != BZ_STREAM_END) {
        strm.next_out = outbuf;
        strm.avail_out = BUFSIZE;
        ret = BZ2_bzDecompress(&strm);
        write(fdout, outbuf, BUFSIZE - strm.avail_out);
    }

    close(fdin);
    close(fdout);
    return 0;

errout:
    return -1;
}

/* ------------------------------------------------------------------------- */

#ifndef NO_MAIN
int main(int argc, char **argv) {
    if (argc == 3) return bzcp(argv[1], argv[2]);
    write(1, "usage: bzcp <infile.bz2> <outfile>\n", 35);
    return -1;
}
#endif

/* ------------------------------------------------------------------------- */
