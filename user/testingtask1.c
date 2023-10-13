#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void test_priority(int priority) {
    int pid = getpid();

    printf("Process %d with priority %d is running.\n", pid, priority);

    int my_priority = getpriority();

    if (my_priority == priority) {
        printf("Process %d has the correct priority %d.\n", pid, my_priority);
    } else {
        printf("Process %d has an incorrect priority %d. Expected %d.\n", pid, my_priority, priority);
    }

    sleep(100); // Sleep for a while to give other processes a chance to run
    exit(0);
}

int main() {
    int num_procs = 5;
    int priorities[] = {10, 20, 30, 40, 50};

    for (int i = 0; i < num_procs; i++) {
        int pid = fork();
        if (pid == 0) {
            setpriority(priorities[i]);
            test_priority(priorities[i]);
        }
    }

    // Parent process waits for all child processes to finish.
    for (int i = 0; i < num_procs; i++) {
        wait(0);
    }

    exit(0);
}
