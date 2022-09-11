#include "user.h"

int main()
{
  printf("pid=%d\n", getpid());

  traceme(1);
  sbrk(4);
  getpid();
  traceme(0);
  puts("end");
  exit(0);
}
