/*
 * fnmatch.h
 *
 * simple prototype and flags, October 2006, by Ivo van Poorten
 */

#ifndef LIBCOMMON_FNMATCH_H
#define LIBCOMMON_FNMATCH_H

int fnmatch(const char *, const char *, int);

#define FNM_NOMATCH     (1 << 0)
#define FNM_PATHNAME    FNM_NOMATCH
#define FNM_PERIOD      (1 << 1)
#define FNM_NOESCAPE    (1 << 2)
#define FNM_NOSYS       (-1)

#endif
