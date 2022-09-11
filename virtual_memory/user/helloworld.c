#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    puts("\n Hello, world!! \n");
    vmprintmappings();
    return 1;
}