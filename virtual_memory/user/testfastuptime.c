#include "user.h"

uint fastuptime() __attribute__((weak));

int
main(int argc, char **argv)
{
  if(argc < 2){
    fprintf(2, "Error: Test needs an input!\n");
    exit(1);
  }

  if (fastuptime){
    vmprintmappings();
    sleep(atoi(argv[1]));
    printf("Time (uptime):%d\n", uptime());
    printf("Time (fastuptime):%d\n", fastuptime());
    printf("end");
  } else {
    printf("Error: fastuptime not implemented yet.\n");
  }

  exit(0);
}
