#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
//#include "kalloc.c" // task 1

// Page table entry flags task 1
#define PTE_P 0x001 // Present
#define PTE_PG 0x080 // Page Global
// maps a physical address to a virtual address,
void *P2V(uint64 pa) {
    return (void *)(pa + KERNBASE);
}
// task 1 end

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


// Task 1 freepmeme
uint64
sys_freepmem(void)
{
    int addr; // The virtual memory address to free
    if (argint(0, &addr) < 0) // argint is used to retrieve the virtual memory address from the user
        return -1;

    // Check if addr is a valid virtual address within the heap range
    if (addr < myproc()->sz || addr >= KERNBASE)
        return -1;

    // Calculate the page number and ensure it's page-aligned
    uint64 pagenum = PGROUNDDOWN(addr) / PGSIZE;

    // Get the current process as a pointer
    struct proc *curproc = myproc();

    // Lock the process to prevent race conditions with other processes
    acquire(&curproc->lock);

    // Ensure the page is allocated and is part of the heap
    if (curproc->pagetable[pagenum] & PTE_P && (curproc->pagetable[pagenum] & PTE_PG)) {
        // Free the physical memory frame (unmap the page)
        kfree((char*)P2V(curproc->pagetable[pagenum] & ~PTE_P));

        // Mark the page table entry as not present
        curproc->pagetable[pagenum] &= ~PTE_P;

        // Decrease the size of the process's heap
        curproc->sz -= PGSIZE;

        // Release the process lock
        release(&curproc->lock);

        return 0; // Return success
    }

    // If the page was not found or not allocated, release the process lock and return an error
    release(&curproc->lock);
    return -1;
}
