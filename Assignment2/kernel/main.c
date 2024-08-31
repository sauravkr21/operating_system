#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if(cpuid() == 0){
    consoleinit();
    printfinit();
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");
    kinit();         // physical page allocator
    kvminit();       // create kernel page table
    kvminithart();   // turn on paging
    procinit();      // process table
    trapinit();      // trap vectors
    trapinithart();  // install kernel trap vector
    plicinit();      // set up interrupt controller
    plicinithart();  // ask PLIC for device interrupts
    binit();         // buffer cache
    iinit();         // inode table
    fileinit();      // file table
    virtio_disk_init(); // emulated hard disk
    userinit();      // first user process
    __sync_synchronize();
    started = 1;
  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plicinithart();   // ask PLIC for device interrupts
  }

  //initialization
  struct cpu *c = &cpus[cpuid()];
  c->sched_policy = 2;
  c->nump = 0;
  c->comp = 0;
  c->stime = -1;
  c->tatime = 0;
  c->wtime = 0;
  c->ctime = 0;
  c->max_ctime = 0;
  c->min_ctime = 1000000000;
  c->nbursts = 0;
  c->max_blen = 0;
  c->min_blen = 1000000000;
  c->tblen = 0;
  c->nebursts = 0;
  c->max_belen = 0;
  c->min_belen = 1000000000;
  c->teblen = 0;
  c->ebursts = 0;
  c->tebursts = 0;

  scheduler();        
}
