/*
 * progmtr.c
 *
 * Display progress meter
 *
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

/* --------------------------------------------------------------------------*/

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "version.h"

#define BUFSIZE 8192

#define CURSOR_LEFT     "\033[1D"
#define CURSOR_4LEFT    "\033[4D"

static char *buf[BUFSIZE];

typedef enum type_e { TYPE_PERC, TYPE_BAR } type_t;

type_t type = TYPE_PERC;
unsigned long total = 0L, length = 77;

int help(char *name) {
    fprintf(stderr,
        "progress meter %s -- Copyright (c) 2006, Ivo van Poorten\n"
        "usage: %s [options] <amount>\n\n"
        "<amount>            amount of data coming from stdin\n\n"
        "-b                  print a progress bar\n"
        "-p                  print percentages (default)\n"
        "--length=LENGTH     specify length of progress bar (default: 77)\n"
        "--usage             print short usage description\n"
        "-?|-help|--help     print this help message\n\n", version, name);
    return 2;
}

int usage(char *name) {
    fprintf(stderr, "usage: %s [-b] [-p] [--length=LENGTH] [--usage] ", name);
    fprintf(stderr, "[--help] <amount>\n");
    return 2;
}

int parse_cmdline(int argc, char **argv) {
    int c = 1;

    while (c != argc) {
        if (total != 0L) {
            fprintf(stderr, "%s: garbage after total amount\n", argv[0]);
            return 2;
        }
        if (!strcmp(argv[c],  "-p"))
            type = TYPE_PERC;
        else if (!strcmp(argv[c],  "-b"))
            type = TYPE_BAR;
        else if (!strncmp(argv[c], "--length=", 9))
            length = strtoul(&argv[c][9], NULL, 10);
        else if (!strcmp(argv[c], "--usage"))
            return usage(argv[0]);
        else if (!strcmp(argv[c], "--help"))
            return help(argv[0]);
        else if (!strcmp(argv[c], "-help"))
            return help(argv[0]);
        else if (!strcmp(argv[c], "-?"))
            return help(argv[0]);
        else if (!strncmp(argv[c], "-", 1)) {
            fprintf(stderr, "%s: unknown command line option %s\n",
                    argv[0], argv[c]);
            return 2; }
        else total = strtoul(argv[c], NULL, 10);
        c++;
    }

    if (total == 0L) {
        fprintf(stderr, "%s: total amount not specified\n", argv[0]);
        return 2;
    }

    return 0;
}

int main(int argc, char **argv) {
    size_t x;
    unsigned long t, counter = 0L, p = 0L, pp = 0L;
    int r, i, j;

    r = parse_cmdline(argc, argv);
    if (r != 0) exit(r);
    t = total;

    while(1) {
        x = read(STDIN_FILENO, buf, BUFSIZE);
        if (x == 0) break;
        write(STDOUT_FILENO, buf, x);
        counter += x;
        p = counter * 100 / t;
        if (p > 100) p = 100;
        if (p == pp)    continue;
        pp = p;
        if (type == TYPE_PERC) {
            fprintf(stderr, "%3ld%%%s", p, CURSOR_4LEFT);
            fflush(stderr);
        } else {
            i = p * length / 100;
            fputc('[', stderr);
            for (j=0; j<i; j++)             fputc('*', stderr);
            for (j=0; j<(length-i); j++)    fputc('-', stderr);
            fputc(']', stderr);
            for (j=0; j<(length+2); j++)    fprintf(stderr, CURSOR_LEFT);
            fflush(stderr);
        }
    }
}
