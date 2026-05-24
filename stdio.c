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