// Libc for Popcorn OS
// An attempt to port a general-purpose subset of libc to Popcorn OS
// Current status: 20/47 functions, 1/1 structs, 1/1 macros, 0/3 globals
//
// NOTES: stdout, stdin, stderr do not play nice and are marked UNIMPLEMENTED
//
// This port could not be possible without the work of Sean Barrett,
// who made stb_sprintf. Thanks, stb! It would also not be possible without
// the GNU Project, which created GCC, which made possible the MinGW project, which made
// possible TinyCC by Fabrice Bellard, from which the libc headers have been taken
// (with some modifications).

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
void *malloc(size_t size) { return svc->memalloc(svc, size); }
void free(void *ptr) { svc->memfree(svc, ptr); }

void *calloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    void *p = svc->memalloc(svc, total);
    if (p) memset(p, 0, total);
    return p;
}

void *realloc(void *ptr, size_t size) {
    void *newptr = svc->memalloc(svc, size);
    if (ptr && newptr) {
        // need to know old size; for now copy conservatively
        memcpy(newptr, ptr, size);
        svc->memfree(svc, ptr);
    }
    return newptr;
}
int atoi(const char *nptr) { return 0; }
long int strtol(const char *nptr, char **endptr, int base) { return 0; }
unsigned long int strtoul(const char *nptr, char **endptr, int base) { return 0; }
pop_DEPRECATED("exit is not functioning as of now.") void exit(int) {}

/* stdio.h */
FILE *fopen(const char *path, const char *mode) {
    BOOL r = FALSE, w = FALSE, c = FALSE, b = FALSE;
    for (size_t i = 0; mode[i]; i++) {
        if (mode[i] == 'r') r = TRUE;
        if (mode[i] == 'w') { w = TRUE; c = TRUE; }
        if (mode[i] == '+') { r = TRUE; w = TRUE; }
        if (mode[i] == 'b') b = TRUE;
    }
    FILE *fp = malloc(sizeof(FILE));
    CHAR16 *wpath = _sbloat(path);
    fp->_hndl = svc->fileopen(svc, wpath, (popf_FileMode){ r, w, c, b });
    if (!fp->_hndl) { free(fp); return NULL; }
    fp->pos = 0;
    return fp;
}

int fclose(FILE *stream) {
    return stream->_hndl->close(stream->_hndl);
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t total = size * nmemb;
    void *buf = stream->_hndl->read(stream->_hndl);
    if (!buf) return 0;
    memcpy(ptr, buf, total);
    svc->memfree(svc, buf);
    stream->pos += total;
    return nmemb;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t total = size * nmemb;
    int res;
    if (!stream->_hndl->mode.bytes) {
        char* crlf = _scrlf((char*)ptr);
        CHAR16* wcrlf = _sbloat(crlf);
        free(crlf);
        res = stream->_hndl->write(stream->_hndl, wcrlf);
        free(wcrlf);
    } else {
        res = stream->_hndl->write(stream->_hndl, (void*)ptr);
    }
    if (res < 0) return 0;
    stream->pos += total;
    return nmemb;
}
FILE *fdopen(int fildes, const char *mode) { return NULL; }
FILE *freopen(const  char *path, const char *mode, FILE *stream) { return NULL; }
int fgetc(FILE *stream) { return 0; }
char *fgets(char *s, int size, FILE *stream) { return NULL; }
int getc(FILE *stream) { return 0; }
int getchar(void) {}
char *gets(char *s) { return NULL; }
int ungetc(int c, FILE *stream) { return 0; }
int fflush(FILE *stream) { return 0; }
int putchar (int c) { return 0; }

/* use stb_sprintf.h */
int printf(const char *format, ...) {
    char buf[1024];
    va_list args;
    va_start(args, format);
    int len = stbsp_vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    char* crlf = _scrlf(buf);
    CHAR16* wcrlf = _sbloat(crlf);
    free(crlf);
    svc->print(svc, wcrlf);
    free(wcrlf);
    return len;
}

int fprintf(FILE *stream, const char *format, ...) {
    char buf[1024];
    va_list args;
    va_start(args, format);
    int len = stbsp_vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    CHAR16* wbuf = _sbloat(buf);
    stream->_hndl->write(stream->_hndl, wbuf);
    free(wbuf);
    return len;
}
int sprintf(char *str, const char *format, ...) { return 0; }
int snprintf(char *str, size_t size, const  char  *format, ...) { return 0; }
int asprintf(char **strp, const char *format, ...) { return 0; }
int dprintf(int fd, const char *format, ...) { return 0; }
int vprintf(const char *format, va_list ap) { return 0; }
int vfprintf(FILE  *stream,  const  char *format, va_list ap) { return 0; }
int vsprintf(char *str, const char *format, va_list ap) { return 0; }
int _vsnprintf(char *str, size_t size, const char  *format, va_list ap) { return 0; }
int _vsnwprintf(wchar_t *_Dest,size_t _Count,const wchar_t *_Format,va_list _Args) { return 0; }
int vasprintf(char  **strp,  const  char *format, va_list ap) { return 0; }
int vdprintf(int fd, const char *format, va_list ap) { return 0; }

void perror(const char *s) { /* noop */ }

int puts(const char *s) {
    char* crlf = _scrlf(s);
    CHAR16* wcrlf = _sbloat(crlf);
    free(crlf);
    svc->println(svc, wcrlf);
    free(wcrlf);
    return strlen(s);
}

/* string.h */
size_t strlen(const char *s) { size_t n=0; while(s[n]) n++; return n; }
void *memcpy(void *d, const void *s, size_t n) {
    unsigned char *dd=d; const unsigned char *ss=s;
    for(size_t i=0;i<n;i++) dd[i]=ss[i]; return d;
}
void *memset(void *s, int c, size_t n) {
    unsigned char *p=s; for(size_t i=0;i<n;i++) p[i]=(unsigned char)c; return s;
}
char *strcpy(char *d, const char *s) {
    char *p=d; while((*p++=*s++)); return d;
}
char *strcat(char *dest, const char *src) {
    char *d = dest;
    while (*d) d++;              // move to end of dest
    while ((*d++ = *src++));     // copy src including null terminator
    return dest;
}

char *strchr(const char *s, int c) {
    while (*s) {
        if (*s == (char)c) return (char*)s;
        s++;
    }
    return (c == 0) ? (char*)s : NULL;
}

char *strrchr(const char *s, int c) {
    const char *last = NULL;
    while (*s) {
        if (*s == (char)c) last = s;
        s++;
    }
    if (c == 0) return (char*)s;
    return (char*)last;
}

void *memmove(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    if (d < s) {
        for (size_t i = 0; i < n; i++) d[i] = s[i];
    } else {
        for (size_t i = n; i > 0; i--) d[i-1] = s[i-1];
    }
    return dest;
}

char *strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);
    if (copy) memcpy(copy, s, len);
    return copy;
}
int _strnicmp(const char *_Str1,const char *_Str2,size_t _MaxCount) { return 0; }
int _stricmp(const char *_Str1,const char *_Str2) { return 0; }

#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"