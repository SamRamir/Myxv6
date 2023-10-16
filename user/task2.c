//task 2 test
#include "kernel/types.h"
#include "user/user.h"

int main() {
    int i;

    for (i = 0; i < 10; i++) {
        if (fork() == 0) {
            sleep(100);  // Sleep to keep the process in RUNNABLE state
            exit(0);
        }
    }

    for (i = 0; i < 10; i++) {
        wait(0);
    }

    exit(0);
}
