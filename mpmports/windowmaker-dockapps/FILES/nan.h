/* Some stuff that is missing from Minix -- Oct. 2006, Ivo van Poorten */

#ifndef NAN_H
#define NAN_H

static union {
    unsigned long __l;
    float __f;
} nan_union = 0x7fc00000UL;

#define NAN (nan_union.__f)
#define isnan(x) ( x == NAN ? 1 : 0 )

/* not a true trunc (long can overflow) but sufficient for this application */
#define trunc(x) ((double)((long)(x)))
#define round(x) (isnan(x) ? (x) : (x)>0.0 ? trunc((x)+0.5) : trunc((x)-0.5))
                 
#endif
