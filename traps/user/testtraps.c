#include "user.h"

#define ALLOC_SIZE 10000

int main() {
   // vmprintmappings();
	
   long unsigned int alloc_size = ALLOC_SIZE * sizeof(int);
   printf("--- Allocating array ---\nNum bytes: %d\n", alloc_size);
   int *area = malloc(alloc_size);
   printf("Array var at %p\nFirst entry at %p\n", &area, area);

   printf("--- Initial page tables: ---\n");
   vmprintmappings();


   printf("--- Accessing array[0] @ %p... ---\n", &area[0]);
   area[0] = 'a';
   // vmprintmappings();

   printf("--- Accessing array[%d] @ %p... ---\n", ALLOC_SIZE/2, &area[ALLOC_SIZE/2]);
   area[ALLOC_SIZE/2] = 'a';
   // vmprintmappings();

   printf("--- Accessing array[%d] @ %p... ---\n", ALLOC_SIZE-1, &area[ALLOC_SIZE-1]);
   area[ALLOC_SIZE-1] = 'b';


   printf("--- Final page tables: ---\n");
   vmprintmappings();

   printf("done\n");

	exit(0);
}
