#include "user.h"

int main()
{
  sbrk(4);
  getpid();
  traceme(0);
  printf("pid=%d\n", getpid());
  puts("end");
  exit(0);
}
