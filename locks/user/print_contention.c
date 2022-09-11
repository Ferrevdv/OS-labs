#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  if (argc < 2) {
    printf("Usage: %s executable [args]...\n", argv[0]);
    return 1;
  }


  int pid = fork();
  if(pid == 0){
    exec(argv[1], argv + 1);
    // exec() only returns if there was an error
    printf("Error executing %s\n", argv[1]);
    return 1;
  }else{
      wait(0);
      printlockcontention();
  }

  return 0;
}
