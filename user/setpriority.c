#include "kernel/types.h"
#include "user/user.h"
#include "kernel/pstat.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: my_program <new_priority>\n");
        exit(1);
    }

    int newPriority = atoi(argv[1]);
    if (newPriority <= 0) {
        printf("Invalid\n");
        exit(1);
    }

    int pid;
    struct pstat pstat;

    if (getprocs(&pstat) < 0) {
        printf("Error\n");
        exit(1);
    }

    for (int i = 0; i < NPROC; i++) {
        if (strcmp(pstat.name[i], "sh") == 0) {
            pid = pstat.pid[i];
            if (setpriority(pid, newPriority) < 0) {
                printf("Error\n");
                exit(1);
            }
            break; // Found and set the priority for sh, no need to continue the loop
        }
    }

    if (getprocs(&pstat) < 0) {
        printf("Error");
        exit(1);
    }

    for (int i = 0; i < NPROC; i++) {
        printf("pid: %d, name: %s, priority: %d\n", pstat.pid[i], pstat.name[i], pstat.priority[i]);
    }

    exit(0);
}

