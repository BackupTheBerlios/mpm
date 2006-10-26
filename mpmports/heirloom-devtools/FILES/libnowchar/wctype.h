/*
 * copyright (c) 2006, Ivo van Poorten
 *
 */

#ifndef	LIBNOWCHAR_WCTYPE_H
#define	LIBNOWCHAR_WCTYPE_H

#include <ctype.h>
#include "wchar.h"

#define	iswalnum(c)     isalnum(c)
#define	iswalpha(c)     isalpha(c)
#define	iswblank(c)     isblank(c)
#define	iswcntrl(c)     iscntrl(c)
#define	iswdigit(c)     isdigit(c)
#define	iswgraph(c)     isgraph(c)
#define	iswlower(c)     islower(c)
#define	iswprint(c)     isprint(c)
#define	iswpunct(c)     ispunct(c)
#define	iswspace(c)     isspace(c)
#define	iswupper(c)     isupper(c)
#define	iswxdigit(c)    isxdigit(c)

#define	towlower(c)	(tolower(c) & 0377)
#define	towupper(c)	(toupper(c) & 0377)

#endif
