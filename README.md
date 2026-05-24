# Popcorn OS SDK

Popcorn OS Software Development Kit. Currently only contains a half-working libc subset shim.

## Okay, what's in this shim?

Currently, the subset has 45 functions and 3 globals. Only 22 functions and no globals are implemented. These are listed below.

`gl` stands for global and `fn` stands for function. :white_check_mark: means implemented and :x: means uninplemented. Unimplementable things are :white_check_mark: because we might as well not worry about implementing them.

### `stdlib.h`

- :white_check_mark: fn: `void *calloc(size_t nmemb, size_t size);`
- :white_check_mark: fn: `void *malloc(size_t size);`
- :white_check_mark: fn: `void free(void *ptr);`
- :white_check_mark: fn: `void *realloc(void *ptr, size_t size);`
- :x: fn: `int atoi(const char *nptr);`
- :x: fn: `long int strtol(const char *nptr, char **endptr, int base);`
- :x: fn: `unsigned long int strtoul(const char *nptr, char **endptr, int base);`
- :white_check_mark: fn: `void exit(int);` (Unimplementable: Popcorn OS doesn't have a way to exit other than returning from `pop_main`.)

### `stdio.h`

- :x: gl: `FILE *stdin`
- :x: gl: `FILE *stdout`
- :x: gl: `FILE *stderr`
- :white_check_mark: fn: `FILE *fopen(const char *path, const char *mode)`
- :white_check_mark: fn: `FILE *fdopen(int fildes, const char *mode)` (Unimplementable: Popcorn OS has no concept of file descriptor numbers.)
- :x: fn: `FILE *freopen(const  char *path, const char *mode, FILE *stream)`
- :white_check_mark: fn: `int fclose(FILE *stream)`
- :white_check_mark: fn: `size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)`
- :white_check_mark: fn: `size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)`
- :x: fn: `int fgetc(FILE *stream)`
- :x: fn: `char *fgets(char *s, int size, FILE *stream)`
- :x: fn: `int getc(FILE *stream)`
- :x: fn: `int getchar(void)`
- :x: fn: `char *gets(char *s)`
- :x: fn: `int ungetc(int c, FILE *stream)`
- :x: fn: `int fflush(FILE *stream)`
- :x: fn: `int putchar(int c)`
- :white_check_mark: fn: `int puts(const char* s)`
- :white_check_mark: fn: `int printf(const char *format, ...)`
- :white_check_mark: fn: `int fprintf(FILE *stream, const char *format, ...)`
- :x: fn: `int sprintf(char *str, const char *format, ...)`
- :x: fn: `int snprintf(char *str, size_t size, const  char  *format, ...)`
- :x: fn: `int asprintf(char **strp, const char *format, ...)`
- :x: fn: `int dprintf(int fd, const char *format, ...)`
- :x: fn: `int vprintf(const char *format, va_list ap)`
- :x: fn: `int vfprintf(FILE  *stream,  const  char *format, va_list ap)`
- :x: fn: `int vsprintf(char *str, const char *format, va_list ap)`
- :x: fn: `int vsnprintf(char *str, size_t size, const char  *format, va_list ap)`
- :x: fn: `int vasprintf(char  **strp,  const  char *format, va_list ap)`
- :x: fn: `int vdprintf(int fd, const char *format, va_list ap)`
- :x: fn: `void perror(const char *s)`

### `string.h`

- :white_check_mark: fn: `char *strcat(char *dest, const char *src)`
- :white_check_mark: fn: `char *strchr(const char *s, int c)`
- :white_check_mark: fn: `char *strrchr(const char *s, int c)`
- :white_check_mark: fn: `char *strcpy(char *dest, const char *src)`
- :white_check_mark: fn: `void *memcpy(void *dest, const void *src, size_t n)`
- :white_check_mark: fn: `void *memmove(void *dest, const void *src, size_t n)`
- :white_check_mark: fn: `void *memset(void *s, int c, size_t n)`
- :white_check_mark: fn: `char *strdup(const char *s)`
- :white_check_mark: fn: `size_t strlen(const char *s)`

## Building

Run `make` with GCC installed and it will produce a `libc.o` and a `libctest.bin`. `libc.o` is an object file containing the libc implementation. It is compiled from `libc.c`. `libctest.bin` is a binary you can run in Popcorn OS that tests out the libc. It is compiled from `libctest.c`.

## Credits

This port could not be possible without the work of Sean Barrett, who made stb_sprintf. Thanks, stb! It would also not be possible without the GNU Project, which created GCC, which made possible the MinGW project, which made possible TinyCC by Fabrice Bellard, from which the libc headers have been taken (with some modifications).