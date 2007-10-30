/*
 * lazybox.c
 *
 * Multi-call binary wrapper for Minix src/commands/simple
 *
 * Copyright (c) 2006, 2007, Ivo van Poorten
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

#include "version.h"
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
    char *myname = argv[0];

    x = strlen(argv[0]);
    while (x!=0 && argv[0][x] != '/')   x--;
    if (x!=0) x++;

    if (!strcmp(&argv[0][x], "lazybox")) {
        argv = &argv[1];
        argc--;
        x = 0;
    }

    /* privileges we will drop to */
    uid=getuid();
    gid=getgid();

    for (n=0; sugidbins[n].a; n++)
        if (!strcmp(sugidbins[n].a, &argv[0][x])) sugid = n;

    if (sugid >= 0) { /* drop to listed privileges */
        if (sugidbins[sugid].u) {
            pwd = getpwnam(sugidbins[sugid].u);
            if (pwd) uid = pwd->pw_uid;
        }
        if (sugidbins[sugid].g) {
            grp = getgrnam(sugidbins[sugid].g);
            if (grp) gid = grp->gr_gid;
        }
    }

    setegid(gid);
    seteuid(uid);

    if (0) x=x;
#include "elseif.h"

    fprintf(stderr, "%s %0.2f can be called as:\n%s\n", myname, VERSION, ALLNAMES);
    return 1;
}

