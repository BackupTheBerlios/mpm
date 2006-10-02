/*
 * lazybox.c
 *
 * Copyright (c) 2006, Ivo van Poorten
 *
 * BSD-style license
 *
 * multi-call binary wrapper for Minix src/commands/simple
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

#include "allnames.h"

#define REGISTER(a) int main_##a(int argc, char **argv)

#include "register.h"

#define RETURNMAIN(a)   return main_##a(argc,argv)
#define ELSEIF(a)       else if (!strcmp(&argv[0][x],#a)) RETURNMAIN(a)
#define ELSEIF2(a,b)    else if (!strcmp(&argv[0][x],#a)) RETURNMAIN(b)

typedef struct aug_s {  /* applications, user, group */
    char *a, *u, *g;
} aug_t;

static aug_t sugidbins[] = {
    { "arp",            "root",     NULL    },   
    { "at",             "root",     NULL    },
    { "hostaddr",       "root",     NULL    },
    { "install",        "root",     NULL    },
    { "lpd",            "daemon",   NULL    },
    { "mail",           "root",     NULL    },
    { "passwd",         "root",     NULL    },
    { "pr_routes",      "root",     NULL    },
    { "progressbar",    "root",     NULL    },
    { "df",             "root",     NULL    },
    { "mount",          "root",     NULL    },
    { "su",             "root",     NULL    },
    { "umount",         "root",     NULL    },
    { "ifconfig",       "root",     NULL    },
    { "ping",           "root",     NULL    },
    { "pwdauth",        "root",     NULL    },
    { "install",        "root",     NULL    },
    { "term",           NULL,       "uucp"  },
    { "write",          NULL,       "tty"   },
    { NULL, NULL, NULL }
};

int main(int argc, char **argv) {
    int x, n = 0, sugid = -1;
    uid_t uid;
    gid_t gid;
    struct passwd *pwd;
    struct group *grp;

    x = strlen(argv[0]);
    while (x!=0 && argv[0][x] != '/')   x--;
    if (x!=0) x++;

    /* privileges we will drop to */
    uid=getuid();
    gid=getgid();

    for (n=0; sugidbins[n].a; n++)
        if (!strcmp(sugidbins[n].a, &argv[0][x])) sugid = n;

    if (sugid >= 0) { /* drop to listed privileges */
        if (sugidbins[sugid].u) {
            pwd = getpwnam(sugidbins[sugid].u);
            uid = pwd->pw_uid;
        }
        if (sugidbins[sugid].g) {
            grp = getgrnam(sugidbins[sugid].g);
            gid = grp->gr_gid;
        }
    }

    setegid(gid);
    seteuid(uid);

    if (0) x=x;
#include "elseif.h"

    fprintf(stderr, "%s can be called as:\n%s\n", argv[0], ALLNAMES);
    return 1;
}

