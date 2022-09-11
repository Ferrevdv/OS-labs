#include "user/user.h"
#include "kernel/riscv.h"

#define NWORKERS 3
#define NALLOCLOOPS 10
#define NALLOCDEALLOC 10000

static uint alloc_all_mem()
{
  uint alloced_pages = 0;

  while (1) {
    void* page = sbrk(PGSIZE);

    if (page == (void*)-1) {
      // Out of memory
      break;
    }

    // Make sure page is mapped if lazy-sbrk is enabled
    *(int*)page = 0;

    alloced_pages++;
  }

  // Deallocate all pages
  if (sbrk(-alloced_pages * PGSIZE) == (void*)-1) {
    printf("Failed to deallocate all allocated pages\n");
    exit(1);
  }

  return alloced_pages;
}

static void alloc_dealloc()
{
  for (uint i = 0; i < NALLOCDEALLOC; ++i) {
    void* page = sbrk(PGSIZE);

    if (page != (void*)-1) {
      // Make sure page is mapped if lazy-sbrk is enabled
      *(int*)page = 0;
      sbrk(-PGSIZE);
    }
  }
}

static void alloc_loop(int oom)
{
  for (uint i = 0; i < NALLOCLOOPS; ++i) {
    if (oom) {
      alloc_all_mem();
    } else {
      alloc_dealloc();
    }
  }
}

static void create_worker(int oom)
{
  int pid = fork();

  if (pid < 0) {
    printf("fork() failed\n");
    exit(1);
  } else if (pid == 0) {
    alloc_loop(oom);
    exit(0);
  }
}

static void usage(const char* name)
{
  printf("Usage: %s [--oom]\n", name);
  exit(1);
}

int main(int argc, char** argv)
{
  int oom = 0;

  if (argc == 2) {
    if (strcmp(argv[1], "--oom") == 0) {
      oom = 1;
    } else {
      usage(argv[0]);
    }
  } else if (argc > 2) {
    usage(argv[0]);
  }

  uint num_free_pages_start = alloc_all_mem();
  printf("[stressmem] start: #free pages %d\n", num_free_pages_start);

  for (uint i = 0; i < NWORKERS; ++i) {
    create_worker(oom);
  }

  for (uint i = 0; i < NWORKERS; ++i) {
    wait(0);
  }

  uint num_free_pages_end = alloc_all_mem();
  printf("[stressmem] end: #free pages %d\n", num_free_pages_end);

  if (num_free_pages_start != num_free_pages_end) {
    printf("[stressmem] Lost pages\n");
    exit(1);
  }

  return 0;
}
