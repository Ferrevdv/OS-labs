#ifndef XV6_KERNEL_PERF_H
#define XV6_KERNEL_PERF_H

#include "spinlock.h"

void perf_register_spinlock(struct spinlock* lock);
void perf_print_spinlocks(void);

#endif
