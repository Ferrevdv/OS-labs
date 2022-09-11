#include "user.h"

int main()
{
    int fd, n, i;
    char *buf;
    printf("--- MAPPINGS INIT ---\n");
    vmprintmappings();

    buf = sbrk(0x2000);
    //memset(buf, 0, 0x2000);
    printf("--- MAPPINGS SBRK ---\n");
    vmprintmappings();

    fd = open("README", 0);
    if (fd < 0){
        printf("test-fail! error during file open!\n");
        exit(1);
    }

    n = read(fd, buf, 87);
    if (n < 0){
        printf("test-fail! error during file read!\n");
        exit(1);
    }
    printf("--- MAPPINGS READ ---\n");
    vmprintmappings();

    printf("--- FILE CONTENTS ---\n");
    for (i=0; i < n; i++)
        printf("%c", buf[i]);
    printf("\n");

    return 0;
}