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