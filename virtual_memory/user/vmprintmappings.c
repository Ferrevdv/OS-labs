#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    vmprintmappings();
    sbrk(1);
    puts("\n *** \n");
    vmprintmappings();
    return 1;
}