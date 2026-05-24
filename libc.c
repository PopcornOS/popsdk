#include "popcorn.h"
#include <stddef.h>
#include <stdarg.h>

#include "stb_sprintf.h"

#ifdef _LIBC_DEBUG_ON
    #define _WIDELIT0(x) L##x
    #define _WIDELIT(x) _WIDELIT0(x)
    #define _LIBC_DEBUG(msg) \
        svc->print(svc, L"[libc: " _WIDELIT(__FILE__) L":"); \
        svc->printint(svc, __LINE__); \
        svc->println(svc, L"] " _WIDELIT(msg));
#else
    #define _LIBC_DEBUG(msg)
#endif

/// Declarations

// Libc

/* stdlib.h */
void *calloc(size_t nmemb, size_t size);
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
int atoi(const char *nptr);
long int strtol(const char *nptr, char **endptr, int base);
unsigned long int strtoul(const char *nptr, char **endptr, int base);
void exit(int);

/* stdio.h */
struct __lFILE {
    popf_File* _hndl;
    size_t pos;
};
typedef struct __lFILE FILE;
#define EOF (-1)
FILE *stdin;
FILE *stdout;
FILE *stderr;
static FILE _stdin, _stdout, _stderr;
FILE *fopen(const char *path, const char *mode);
FILE *fdopen(int fildes, const char *mode);
FILE *freopen(const  char *path, const char *mode, FILE *stream);
int fclose(FILE *stream);
size_t  fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t  fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int fgetc(FILE *stream);
char *fgets(char *s, int size, FILE *stream);
int getc(FILE *stream);
int getchar(void);
char *gets(char *s);
int ungetc(int c, FILE *stream);
int fflush(FILE *stream);
int putchar (int c);
int puts (const char* s);

int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const  char  *format, ...);
int asprintf(char **strp, const char *format, ...);
int dprintf(int fd, const char *format, ...);
int vprintf(const char *format, va_list ap);
int vfprintf(FILE  *stream,  const  char *format, va_list ap);
int vsprintf(char *str, const char *format, va_list ap);
int vsnprintf(char *str, size_t size, const char  *format, va_list ap);
int vasprintf(char  **strp,  const  char *format, va_list ap);
int vdprintf(int fd, const char *format, va_list ap);

void perror(const char *s);

/* string.h */
char *strcat(char *dest, const char *src);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strcpy(char *dest, const char *src);
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
char *strdup(const char *s);
size_t strlen(const char *s);

// Internal helpers
pop_Services* svc;

static char* _wsflat(const CHAR16* wstr);
static char* _scrlf(const char* str);
static CHAR16* _sbloat(const char* str);

extern int main(int argc, char** argv);
void __main(void); // GCC complains otherwise

/// Entry point
int pop_API pop_main(pop_Services* asvc, int argc, CHAR16** argv) {
    svc = asvc;
    
    _LIBC_DEBUG("creating stdin, stdout, stderr");
    _stdin._hndl  = svc->console;
    _stdout._hndl = svc->console;
    _stderr._hndl = svc->console;

    _LIBC_DEBUG("creating char** argv");
    char **cargv = malloc(argc * sizeof(char*));
    for (int i=0; i<argc; i++) cargv[i] = _wsflat(argv[i]);

    _LIBC_DEBUG("entering main");
    int ret = main(argc, cargv);

    _LIBC_DEBUG("freeing char** argv");
    for (int i=0; i<argc; i++) free(cargv[i]);
    free(cargv);

    _LIBC_DEBUG("returning");
    return ret;
}

void __main(void) { return; } // GCC complains otherwise

/// Definitions

// Internal helpers
static char* _wsflat(const CHAR16* wstr) {
    if (!wstr) return NULL;
    size_t len = 0;
    while (wstr[len] != 0) len++;

    // Worst case: each UTF-16 code unit becomes 3 bytes in UTF-8
    char* out = svc->memalloc(svc, len * 3 + 1);
    if (!out) return NULL;

    char* p = out;
    for (size_t i = 0; i < len; i++) {
        uint16_t wc = wstr[i];
        if (wc < 0x80) {
            *p++ = (char)wc;
        } else if (wc < 0x800) {
            *p++ = 0xC0 | (wc >> 6);
            *p++ = 0x80 | (wc & 0x3F);
        } else {
            *p++ = 0xE0 | (wc >> 12);
            *p++ = 0x80 | ((wc >> 6) & 0x3F);
            *p++ = 0x80 | (wc & 0x3F);
        }
    }
    *p = '\0';
    return out;
}

static char* _scrlf(const char* str) {
    if (!str) return NULL;
    size_t len = 0;
    while (str[len] != 0) len++;

    // Worst case: each character is \n, which needs to be \r\n for len * 2
    char* out = svc->memalloc(svc, len * 2 + 1);
    if (!out) return NULL;

    char* p = out;
    for (size_t i = 0; i < len; i++) {
        char c = str[i];
        if (c == '\n') {
            *p++ = '\r';
            *p++ = '\n';
        } else {
            *p++ = c;
        }
    }
    *p = '\0';
    return out;
}

static CHAR16* _sbloat(const char* str) {
    if (!str) return NULL;

    // Count codepoints
    size_t len = 0;
    const unsigned char* p = (const unsigned char*)str;
    while (*p) {
        if (*p < 0x80) {
            p++;
        } else if ((*p & 0xE0) == 0xC0) {
            p += 2;
        } else if ((*p & 0xF0) == 0xE0) {
            p += 3;
        } else {
            p += 4; // skip unsupported 4-byte sequences
        }
        len++;
    }

    CHAR16* out = svc->memalloc(svc, (len + 1) * sizeof(CHAR16));
    if (!out) return NULL;

    p = (const unsigned char*)str;
    size_t i = 0;
    while (*p) {
        uint32_t codepoint;
        if (*p < 0x80) {
            codepoint = *p++;
        } else if ((*p & 0xE0) == 0xC0) {
            codepoint = ((*p & 0x1F) << 6) | (p[1] & 0x3F);
            p += 2;
        } else if ((*p & 0xF0) == 0xE0) {
            codepoint = ((*p & 0x0F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F);
            p += 3;
        } else {
            codepoint = '?'; // placeholder for 4-byte UTF-8
            p += 4;
        }
        out[i++] = (CHAR16)codepoint;
    }
    out[i] = 0;
    return out;
}

// Libc

/* stdlib.h */
#include "stdlib.c"

/* stdio.h */
#include "stdio.c"

/* string.h */
#include "string.c"

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"