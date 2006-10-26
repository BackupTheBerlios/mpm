/*
 * copyright (c) 2006, Ivo van Poorten
 */

#undef wint_t
#undef wctype_t
#undef wchar_t

#define wint_t int
#define wchar_t char
#define WEOF        -1
#define EILSEQ      (errno)

#undef swprintf
#undef wcscmp
#undef wcscpy
#undef wcslen
#undef ungetwc
#undef getwc
#undef wcstol
#undef fputws
#undef fputwc

#define swprintf    snprintf
#define wcscmp      strcmp
#define wcscpy      strcpy
#define wcslen      strlen
#define ungetwc     ungetc
#define getwc       getc
#define wcstol      strtol
#define fputws      fputs
#define fputwc      fputc
