#include "kernel/types.h"
#include "user/user.h"

extern int main(int argc, char *argv[]);

void _start(int argc, char *argv[])
{
    int return_main = main(argc, argv);
    printf("%d\n", getnumsyscalls());
    exit(return_main);
}