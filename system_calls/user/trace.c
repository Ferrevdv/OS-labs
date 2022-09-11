#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("No executable given as an argument");
        return -1;
    }
    traceme(1);
    if (exec(argv[1], &argv[1]) == -1)
    {
        printf("An error has occured with executing the given file");
    }
    return 0;
}