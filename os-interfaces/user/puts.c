#include "kernel/types.h"
#include "user/user.h"

void puts(const char *str)
{
    write(1, str, strlen(str));
    write(1, "\n", 2);
}