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