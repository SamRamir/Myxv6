#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

int main(void)
{
    int parent_pid = getpid();
    int child_pids[5];

    for (int i = 0; i < 5; i++) {
        int pid = fork();
        if (pid == 0) {
            // Child process
            int priority = 50 + i;  // Assign priorities 50, 51, 52, 53, 54
            setpriority(priority);
            for (volatile int j = 0; j < 100000000; j++);  // Spend some time
            printf(1, "Child %d with priority %d has finished!\n", getpid(), priority);
            exit(0);
        } else if (pid > 0) {
            child_pids[i] = pid;
        } else {
            printf(1, "Fork failed\n");
            exit(0);
        }
    }

    if (getpid() == parent_pid) {
        // Parent process
        for (int i = 0; i < 5; i++) {
            wait(0);
        }
        printf(1, "All children have finished!\n");
    }

    exit(0);
}