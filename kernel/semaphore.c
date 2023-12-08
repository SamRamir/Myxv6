#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "spinlock.h"

#define NSEM 100

struct semtab semtable;
void seminit(void)
{
    // initializing lock
    initlock(&semtable.lock, "semtable");
    // initializing lock for each semaphore
    for (int i = 0; i < NSEM; i++)
    initlock(&semtable.sem[i].lock, "sem");
};

int semalloc(void){
    acquire(&semtable.lock);
    for (int i = 0; i < NSEM; i++){
        // checking if semaphore entry is not being used
        if(semtable.sem[i].valid == 0){
            semtable.sem[i].valid = 1;
            // release lock
            release(&semtable.lock);
            return i;
        }
    }
    release(&semtable.lock);
    return -1;
};

void semdealloc(int index){
    acquire(&semtable.sem[index].lock);
    if(index > -1 && index < NSEM){
        // invaladinting the semaphore entry
        semtable.sem[index].valid = 0;
    }
    release(&semtable.sem[index].lock);
};