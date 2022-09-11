#include "user.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"

void change_file_permissions(char* name, uint permissions){
   // change file permissions
   int retval = chmod(name, permissions);
   printf("Changing file permissions of file %s\n", name);
   if(retval < 0){
      printf("test fail: Chmod does not seem to work. Did you implement it already?\n");
      exit(1);
   } else { 
      printf("test pass: Chmod seems to work.\n");
   }
}

// Creates a file with the given name and permissions and tests access to it
void create_and_test_file(char *name, int init_files, uint permissions, uint forbidden_access_flags, uint allowed_access_flags)
{
   int fd;
   if(init_files){
      if((fd = open(name, O_CREATE|O_WRONLY)) < 0){
         fprintf(1, "Error: cannot open %s. This should not really happen or your files do not persist across reboots.\n", name);
         exit(1);
      }
      // Write a hello to the file
      write(fd, "hello", 5);
      close(fd);
      printf("test info: Created test file %s\n", name);
      
      change_file_permissions(name, permissions);
   }
   // attempt to read/write from the file
   printf("Attempting to access file %s on forbidden permission (the permission in the file name) \n", name);
   if((fd = open(name, forbidden_access_flags)) < 0){
      fprintf(1, "test pass: cannot open %s after changing permissions. Good!\n", name);
   } else {
      fprintf(1, "test fail: Protected file %s can still be opened.\n", name);
      exit(1);
   }
   
   // attempt to read/write from the file but we expect this to work as only the other one was forbidden
   printf("Attempting to access file %s on still allowed permission (read when write is forbidden and write when read is forbidden)\n", name);
   if((fd = open(name, allowed_access_flags)) >= 0){
      fprintf(1, "test pass: can still open %s for the unchanged permission. Good!\n", name);
   } else {
      fprintf(1, "test fail: Wrong permission was removed from file %s and can not be opened again on remaining permission.\n", name);
      char *argv[1];
      argv[0] = "ls";
      exec("ls", argv);
      exit(1);
   }

}

int main(int argc, char *argv[]) {
   if(argc != 4){
      printf("Wrong usage\n");
      exit(1);
   }

   int canary_fd;
   int init_files;

   if((canary_fd = open(".canary", O_RDONLY)) < 0){
      printf("Canary file does not exist, we assume this is the first test run. Creating canary file.\n");
      if((canary_fd = open(".canary", O_CREATE|O_WRONLY)) < 0){
            printf("Error: Cannot create files.\n");
            exit(1);
         } else {
            write(canary_fd, "canary", 6);
            close(canary_fd);
      }
      init_files = 1;
   } else {
      printf("Canary file does exist, we assume this is the second test run.\n");
      char buf[6];
      read(canary_fd, buf, 6);
      if(strcmp(buf, "canary") != 0){
         printf("Error: Canary file did not persist across reboots.\n");
         exit(1);
      } else {
         close(canary_fd);
      }
      init_files = 0;
   }

   create_and_test_file(argv[1], init_files, M_READ, O_WRONLY, O_RDONLY);
   create_and_test_file(argv[2], init_files, M_WRITE, O_RDONLY, O_WRONLY);

   if(init_files){
      // For exec we need something different, let's link 
      // the ls command and disallow execution of the copy.
      link("ls", argv[3]);
      printf("Created a copy of ls\n");
      
      change_file_permissions(argv[3], M_ALL ^ M_EXECUTE);
   }
   printf("Attempting to execute %s. We do not expect that to work!\n", argv[3]);
   if(exec(argv[3], argv + 3) < 0){ // If exec works, we may not reach beyond this point. Catch taht in Python.
      printf("Exec returned\n");
      printf("test pass: exec does not seem to work after removing x permissions. Good!\n");
   }

   // will not be reached if exec passes
   exit(0);
}
