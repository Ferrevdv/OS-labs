#include "kernel/types.h"
#include "user/user.h"

int message_id = 42;
const char *message = "Hello, world";

void print_message()
{
    printf("[%d] %s\n", message_id, message);
}

int main()
{
    vmprintmappings();

    print_message();

    return 0;
}