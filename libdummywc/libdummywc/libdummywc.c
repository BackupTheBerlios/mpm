/* This is a dummy library that "implements" a few wc* functions through
 * their non-wc counterparts, i.e. wcsncpy --> strncpy
 *
 * This only works if you are SURE that no wide character strings are used
 * throughout the application, i.e. the application was configured with
 * --disable-nls but nevertheless still calls wc* functions on 7-bit ASCII
 * strings. One example of such application/library is libXaw from Xorg 7.1.
 *
 * Not sure if this even validates for having a license, so consider this
 * "code" public domain.
 *
 * September 2006, Ivo van Poorten
 */

#include <stddef.h>
#include <string.h>

wchar_t *wcsncpy(wchar_t *dest, const wchar_t *src, size_t n) {
    return strncpy(dest, src, n);
}

size_t wcslen(const wchar_t *s) {
    return strlen(s);
}

wchar_t *wcscpy(wchar_t *dest, const wchar_t *src) {
    return strcpy(dest, src);
}

