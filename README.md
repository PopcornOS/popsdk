# Popcorn OS SDK

Popcorn OS Software Development Kit. Currently only contains a half-working libc subset shim.

## Okay, what's in this shim?

Currently, the subset has 45 functions and 3 globals. Only 22 functions and no globals are implemented. These are listed below.

`gl` stands for global and `fn` stands for function. Checked boxes represent implemented and unchecked ones represent uninplemented. Unimplementable things are checked because we might as well not worry about implementing them.

### `stdlib.h`

- [x] fn: `void *calloc(size_t nmemb, size_t size);`
- [x] fn: `void *malloc(size_t size);`
- [x] fn: `void free(void *ptr);`
- [x] fn: `void *realloc(void *ptr, size_t size);`
- [ ] fn: `int atoi(const char *nptr);`
- [ ] fn: `long int strtol(const char *nptr, char **endptr, int base);`
- [ ] fn: `unsigned long int strtoul(const char *nptr, char **endptr, int base);`
- [x] fn: `void exit(int);` (Unimplementable: Popcorn OS doesn't have a way to exit other than returning from `pop_main`.)

### `stdio.h`

- [ ] gl: `FILE *stdin`
- [ ] gl: `FILE *stdout`
- [ ] gl: `FILE *stderr`
- [x] fn: `FILE *fopen(const char *path, const char *mode)`
- [x] fn: `FILE *fdopen(int fildes, const char *mode)` (Unimplementable: Popcorn OS has no concept of file descriptor numbers.)
- [ ] fn: `FILE *freopen(const  char *path, const char *mode, FILE *stream)`
- [x] fn: `int fclose(FILE *stream)`
- [x] fn: `size_t  fread(void *ptr, size_t size, size_t nmemb, FILE *stream)`
- [x] fn: `size_t  fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)`
- [ ] fn: `int fgetc(FILE *stream)`
- [ ] fn: `char *fgets(char *s, int size, FILE *stream)`
- [ ] fn: `int getc(FILE *stream)`
- [ ] fn: `int getchar(void)`
- [ ] fn: `char *gets(char *s)`
- [ ] fn: `int ungetc(int c, FILE *stream)`
- [ ] fn: `int fflush(FILE *stream)`
- [ ] fn: `int putchar (int c)`
- [x] fn: `int puts (const char* s)`
- [x] fn: `int printf(const char *format, ...)`
- [x] fn: `int fprintf(FILE *stream, const char *format, ...)`
- [ ] fn: `int sprintf(char *str, const char *format, ...)`
- [ ] fn: `int snprintf(char *str, size_t size, const  char  *format, ...)`
- [ ] fn: `int asprintf(char **strp, const char *format, ...)`
- [ ] fn: `int dprintf(int fd, const char *format, ...)`
- [ ] fn: `int vprintf(const char *format, va_list ap)`
- [ ] fn: `int vfprintf(FILE  *stream,  const  char *format, va_list ap)`
- [ ] fn: `int vsprintf(char *str, const char *format, va_list ap)`
- [ ] fn: `int vsnprintf(char *str, size_t size, const char  *format, va_list ap)`
- [ ] fn: `int vasprintf(char  **strp,  const  char *format, va_list ap)`
- [ ] fn: `int vdprintf(int fd, const char *format, va_list ap)`
- [ ] fn: `void perror(const char *s)`

### `string.h`

- [x] fn: `char *strcat(char *dest, const char *src)`
- [x] fn: `char *strchr(const char *s, int c)`
- [x] fn: `char *strrchr(const char *s, int c)`
- [x] fn: `char *strcpy(char *dest, const char *src)`
- [x] fn: `void *memcpy(void *dest, const void *src, size_t n)`
- [x] fn: `void *memmove(void *dest, const void *src, size_t n)`
- [x] fn: `void *memset(void *s, int c, size_t n)`
- [x] fn: `char *strdup(const char *s)`
- [x] fn: `size_t strlen(const char *s)`

## Building

Run `make` with GCC installed and it will produce a `libc.o` and a `libctest.bin`. `libc.o` is an object file containing the libc implementation. It is compiled from `libc.c`. `libctest.bin` is a binary you can run in Popcorn OS that tests out the libc. It is compiled from `libctest.c`.

## Credits

This port could not be possible without the work of Sean Barrett, who made stb_sprintf. Thanks, stb! It would also not be possible without the GNU Project, which created GCC, which made possible the MinGW project, which made possible TinyCC by Fabrice Bellard, from which the libc headers have been taken (with some modifications).