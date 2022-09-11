#include "user/user.h" // in xv6! 
int main(){
  int *p = 0; //p is a pointer to the address 0
  uint val = *p;
  printf("Test fail: The value at address 0 is %d. Protection page seems to not be in place!\n", val);
  exit(0);
}
