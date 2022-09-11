#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (fork() == 0)
    {
        vmprintmappings();
        puts("\n CHILD TERMINATED \n");
    }
    else
    {
        wait(0);
        vmprintmappings();
        puts("\n *** \n");
        char *a[] = {"helloworld", 0};
        exec("helloworld", a);
    }
    return 0;
}