#include "perf.h"

#include "defs.h"

#define NPERFLOCKS 128

static struct spinlock* perf_locks[NPERFLOCKS];

void perf_register_spinlock(struct spinlock* lock) {
  for (uint i = 0; i < NPERFLOCKS; ++i) {
    if (perf_locks[i] == 0) {
      perf_locks[i] = lock;
      return;
    }
  }

  panic("perf_register_spinlock");
}

void perf_print_spinlocks() {
  uint64 total_acquires = 0;
  uint64 total_contention = 0;
  uint num_locks = 0;

  for (uint i = 0; i < NPERFLOCKS; ++i) {
    struct spinlock* lock = perf_locks[i];

    if (lock != 0) {
      printf("[perf] lock=%s, #acquires=%d, contention=%d\n",
             lock->name, (int)lock->num_acquires, (int)lock->contention);

      num_locks++;
      total_acquires += lock->num_acquires;
      total_contention += lock->contention;
    }
  }

  if (num_locks > 1) {
    printf("[perf] totals for shown locks, #acquires=%d, contention=%d\n",
           (int)total_acquires, (int)total_contention);
  }
}

