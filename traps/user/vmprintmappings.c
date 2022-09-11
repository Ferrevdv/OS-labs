#include "kernel/types.h"
#include "user/user.h"

int main()
{
    vmprintmappings();
    int* a = (int *)sbrk(100);
    printf("\n");
    vmprintmappings();
    printf("\n");
    *a = 5;
    vmprintmappings();


    return 0;
}