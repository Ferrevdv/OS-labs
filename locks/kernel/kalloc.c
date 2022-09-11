// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "perf.h"

void freerange(void *pa_start, void *pa_end);
struct run *steal_first_frame(int cpu);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem[NCPU];


void
kinit()
{
  for (int i = 0; i < NCPU; i++)
    initlock(&kmem[i].lock, "kmem");
  freerange(end, (void*)PHYSTOP);
  for (int i = 0; i < NCPU; i++)
    perf_register_spinlock(&kmem[i].lock);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;
  push_off();
  uint cid = cpuid();
  pop_off();

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem[cid].lock);
  r->next = kmem[cid].freelist;
  kmem[cid].freelist = r;
  release(&kmem[cid].lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.

  // Stealing implementation: 
    // 1. Find the first CPU that has a free frames 
    // 2. Try to steel NFRAMES free frames from that CPU
      // Note: if CPU has less than NFRAMES free frames, then don't try to steal from other CPUs
      // This is a concious design choice that can be easily modified if desired
    // 3. Adjust free lists of both CPU's & return first stolen frame

    // Note: deadlock is avoided by never holding more than one lock

void *
kalloc(void)
{
  struct run *r;
  int NFRAMES = 8;
  
  push_off();
  int cid = cpuid();
  
  acquire(&kmem[cid].lock);
  r = kmem[cid].freelist;
  if(r){
    kmem[cid].freelist = r->next;
    release(&kmem[cid].lock);
  }
  else 
  {
    release(&kmem[cid].lock);
    for (int i = 0; i < NCPU; i++)
    {
      if (i != cid){
        struct run* frame = steal_first_frame(i);
        if (frame){   // free list of CPU_i is not empty!
          r = frame;
          int j = 0;
          struct run *temp = r;   // temporary pointer that points to last frame in r
          while (frame && (j < NFRAMES - 1)){  // we try to steal NFRAMES from CPU, if CPU has less free pages, we 
            frame = steal_first_frame(i);
            temp->next = frame;
            temp = temp->next;
            j++;
          }
          break;
        }
      }
    } 
    if (r){   // we managed to steal free frames from other CPU
      acquire(&kmem[cid].lock);
      kmem[cid].freelist = r->next;
      release(&kmem[cid].lock);
    }
  }
  pop_off();

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}

// Steals first frame from cpu & returns this frame
struct run *
steal_first_frame(int cpu)
{
  acquire(&kmem[cpu].lock);
  struct run* stolen_frame = kmem[cpu].freelist;
  if (stolen_frame){
    kmem[cpu].freelist = (kmem[cpu].freelist)->next;
    stolen_frame->next = 0;
  }
  release(&kmem[cpu].lock);

  return stolen_frame;
}