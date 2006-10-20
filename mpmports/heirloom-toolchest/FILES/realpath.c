/*
 * realpath.c
 *
 * Copyright (c) 2006, Ivo van Poorten
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * My name may NOT be used to endorse or promote products derived from
 *      this software without specific prior written permission.
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
 */

#ifdef _MINIX

#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <string.h>
#include "realpath.h"

char *realpath(const char *file_name, char *resolved_name) {
    char cwd[PATH_MAX];
    int save_errno;

    if (file_name == NULL || resolved_name == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        strncpy(resolved_name, file_name, PATH_MAX);
        return NULL;
    }

    if (chdir(file_name) < 0) {
        strncpy(resolved_name, file_name, PATH_MAX);
        return NULL;
    }

    if (getcwd(resolved_name, PATH_MAX) == NULL) {
        save_errno = errno;
        chdir(cwd);
        strncpy(resolved_name, file_name, PATH_MAX);
        errno = save_errno;
        return NULL;
    }

    chdir(cwd);
    return resolved_name;
}

#if 0
#include <stdio.h>
int main(int argc, char **argv) {
    char resolved_path[PATH_MAX];
    char *ret;

    if (argc != 2) return 1;

    ret = realpath(argv[1], resolved_path);
    if (ret == NULL)
        printf("ret=NULL  errno=%i\n", errno);
    else
        printf("resolved_path: %s\n", resolved_path);

    return 0;
}
#endif

#endif
