#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "proc.h"
#include "semaphore.h"
#include "barr.h"
#include "buffer.h"
#include "sem_buffer.h"


struct sleeplock printlock;
int tail = 0;
int head = 0;
int sem_tail = 0;
int sem_head = 0;


uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_getppid(void)
{
  if (myproc()->parent) return myproc()->parent->pid;
  else {
     printf("No parent found.\n");
     return 0;
  }
}

uint64
sys_yield(void)
{
  yield();
  return 0;
}

uint64
sys_getpa(void)
{
  uint64 x;
  if (argaddr(0, &x) < 0) return -1;
  return walkaddr(myproc()->pagetable, x) + (x & (PGSIZE - 1));
}

uint64
sys_forkf(void)
{
  uint64 x;
  if (argaddr(0, &x) < 0) return -1;
  return forkf(x);
}

uint64
sys_waitpid(void)
{
  uint64 p;
  int x;

  if(argint(0, &x) < 0)
    return -1;
  if(argaddr(1, &p) < 0)
    return -1;

  if (x == -1) return wait(p);
  if ((x == 0) || (x < -1)) return -1;
  return waitpid(x, p);
}

uint64
sys_ps(void)
{
   return ps();
}

uint64
sys_pinfo(void)
{
  uint64 p;
  int x;

  if(argint(0, &x) < 0)
    return -1;
  if(argaddr(1, &p) < 0)
    return -1;

  if ((x == 0) || (x < -1) || (p == 0)) return -1;
  return pinfo(x, p);
}

uint64
sys_forkp(void)
{
  int x;
  if(argint(0, &x) < 0) return -1;
  return forkp(x);
}

uint64
sys_schedpolicy(void)
{
  int x;
  if(argint(0, &x) < 0) return -1;
  return schedpolicy(x);
}

uint64
sys_barrier_alloc(void)
{
  return barrier_alloc();
}

uint64
sys_barrier(void)
{
  int inst_num, id, np;
  if(argint(0, &inst_num) < 0) return -1;
  if(argint(1, &id) < 0) return -1;
  if(argint(2, &np) < 0) return -1;
 
  barrier(inst_num, id, np);

  return 0;
}

uint64
sys_barrier_free(void)
{
  int id;
  if(argint(0, &id) < 0) 
      return -1;
  return barrier_free(id);
}

uint64
sys_buffer_cond_init(void)
{
  bufferinit();
  return 0;
}

uint64
sys_cond_produce(void)
{
  int val;
  if(argint(0, &val) < 0) return -1;
  int ind = 0;

  acquiresleep(&insert_lock);
  ind = tail;
  tail = (tail + 1) % NUM_BUFFER;
  releasesleep(&insert_lock);

  acquiresleep(&buf_arr[ind].lock);
  while(buf_arr[ind].full) {
    cond_wait(&buf_arr[ind].deleted, &buf_arr[ind].lock);
  }
  buf_arr[ind].value = val;
  buf_arr[ind].full = 1;
  cond_signal(&buf_arr[ind].inserted);
  releasesleep(&buf_arr[ind].lock);

  return 0;
}

uint64
sys_cond_consume(void)
{
  int ind = 0;
  int val = 0;
  
  acquiresleep(&delete_lock);
  ind = head;
  head = (head + 1) % NUM_BUFFER;
  releasesleep(&delete_lock);

  acquiresleep(&buf_arr[ind].lock);
  while(!buf_arr[ind].full) {
    cond_wait(&buf_arr[ind].inserted, &buf_arr[ind].lock);
  }
  buf_arr[ind].full = 0;
  val = buf_arr[ind].value;
  cond_signal(&buf_arr[ind].deleted);
  releasesleep(&buf_arr[ind].lock);
  acquiresleep(&printlock);
  printf("%d ", val);
  releasesleep(&printlock);

  return val;
}

uint64
sys_buffer_sem_init(void)
{
  sembufferinit();
  return 0;
}

uint64
sys_sem_produce(void){
  int val;
  if(argint(0,&val) < 0) return -1;
  
  sem_wait(&empty);
  sem_wait(&pro);
    sem_buf_arr[sem_tail].value = val;
    sem_tail = (sem_tail + 1) % NUM_SEM_BUFFER;
  sem_post(&pro);
  sem_post(&full);

  return 0;
}

uint64
sys_sem_consume(void){
  int val = 0;
  sem_wait(&full);
  sem_wait(&con);
    val = sem_buf_arr[sem_head].value;
    sem_head = (sem_head + 1) % NUM_SEM_BUFFER;
  sem_post(&con);
  sem_post(&empty);

  acquiresleep(&printlock);
    printf("%d ", val);
  releasesleep(&printlock);

  return val;
}