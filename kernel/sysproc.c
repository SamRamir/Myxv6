#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

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

// return the number of active processes in the system
// fill in user-provided data structure with pid,state,sz,ppid,name
uint64
sys_getprocs(void)
{
  uint64 addr;  // user pointer to struct pstat

  if (argaddr(0, &addr) < 0)
    return -1;
  return(procinfo(addr));
}

// hw 6 task 3 changes

int
sys_sem_init(void){
  uint64 s;
  int index;
  int value;
  int pshared;
  // checking if values are valid
  if (argaddr(0,&s) < 0 || argint(1, &pshared) < 0 || argint(2, &value) < 0)
    return -1;

  if(pshared != 1){
    return -1;
  }
  index = semalloc();
  semtable.sem[index].count = value;
  // copying allocated index semaphore
  if(copyout(myproc()->pagetable, s, (char*)&index, sizeof(index)) <0){
    return -1;
  }
  
  return 0;
}

int
sys_sem_destroy(void)
{
    sem_t* sem;
    
    if (argaddr(0, (void*)&sem) < 0)
        return -1;

    uint64 sem_index;
    // Copying the semaphore index from user space to kernel space
    if (copyin(myproc()->pagetable, (char*)&sem_index, (uint64)sem, sizeof(sem_t)) < 0)
        return -1;

    // Deallocate the semaphore using the obtained index
    semdealloc(sem_index);
    
    return 0;
}


int
sys_sem_wait(void) {
  uint64 s;
  int addr;

  if (argaddr(0, &s) < 0) {
    return -1;
  }

  // Copying the address from user space to kernel space
  copyin(myproc()->pagetable, (char*)&addr, s, sizeof(int));
  
  acquire(&semtable.sem[addr].lock);

  if (semtable.sem[addr].count > 0) {
    semtable.sem[addr].count--; 
    release(&semtable.sem[addr].lock);
    return 0;
  } else {
    while (semtable.sem[addr].count == 0) {
      // using sleep
      sleep((void*)&semtable.sem[addr], &semtable.sem[addr].lock);
    }
    // decreasing count
    semtable.sem[addr].count--;
    release(&semtable.sem[addr].lock);
  }
  
  return 0;
}

int
sys_sem_post(void){
  uint64 s;
  int addr;
  //semaphore failed
  if(argaddr(0, &s) < 0){
    return -1;
  }
  copyin(myproc()->pagetable, (char*)&addr, s, sizeof(int));
  
  acquire(&semtable.sem[addr].lock);
  semtable.sem[addr].count++;
  // using wakeup
  wakeup((void*)&semtable.sem[addr]);
  
  release(&semtable.sem[addr].lock);
  
  return 0;
}