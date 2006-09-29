#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFSIZE 8192

#define CURSOR_LEFT     "\033[1D"
#define CURSOR_4LEFT    "\033[4D"

static char *buf[BUFSIZE];

typedef enum type_e { TYPE_PERC, TYPE_BAR } type_t;

type_t type = TYPE_PERC;
unsigned long total = 0L, length = 77;

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
        else if (!strncmp(argv[c], "-length=", 8))
            length = strtoul(&argv[c][8], NULL, 10);
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
    fputc('\n', stderr);
}

