#include "kernel/types.h"
#include "user/user.h"

extern int main(int argc, char* argv[]);

void _start(int argc, char* argv[])
{
  int exit_code = main(argc, argv);
  exit(exit_code);
}
