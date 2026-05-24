#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "popcorn.h"

int main(int argc, char** argv) {
    // text output
    puts("puts: Hello world!");
    printf("printf: %s Here are some numbers: %d %.1f\n", "Hello world!", 29894, 10.235);
    
    FILE* fp = fopen("/virt/dev/con", "w");
    fwrite("fwrite: Hello world!\n", 22, sizeof(CHAR16), fp);
    fprintf(fp, "fprintf: %s Here are some numbers: %d %.1f\n", "Hello world!", 29894, 10.235);
    fclose(fp);
    
    // argument test
    puts("\nargc and argv test:");
    puts("===================\n");
    for (int i = 0; i < argc; i++) {
        puts(argv[i]);
    }
    
    // library function torture test
    puts("\nlibrary torture test");
    puts("====================\n");

    /* strlen */
    printf("strlen(\"Hello world!\") = %d\n", (int)strlen("Hello world!"));

    /* strcat / strcpy */
    char buf1[64];
    strcpy(buf1, "Hello");
    strcat(buf1, " World!");
    printf("strcpy+strcat result: %s\n", buf1);

    /* strchr / strrchr */
    printf("strchr(\"abcdef\", 'c') = %s\n", strchr("abcdef", 'c'));
    printf("strrchr(\"abcabc\", 'a') = %s\n", strrchr("abcabc", 'a'));

    /* strdup */
    char *dup = strdup("duplicate me");
    printf("strdup result: %s\n", dup);
    free(dup);

    /* memcpy / memset / memmove */
    char src[] = "memcpy test";
    char dst[32];
    memcpy(dst, src, strlen(src)+1);
    printf("memcpy result: %s\n", dst);

    memset(dst, '*', 5);
    dst[5] = '\0';
    printf("memset result: %s\n", dst);

    char overlap[] = "123456789";
    memmove(overlap+2, overlap, 5); /* overlapping copy */
    printf("memmove overlap result: %s\n", overlap);
    
    return 0;
}