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

