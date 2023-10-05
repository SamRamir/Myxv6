// Task 1 time1.c
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: time1 command [args...]\n");
        exit(1);
    }

    // Start time of ticks
    int start_time = uptime();

    //child process for command
    int pid = fork();
    if (pid < 0) {
        fprintf(2, "Fork failed\n");
        exit(1);
    } else if (pid == 0) {
        // child process
        exec(argv[1], argv + 1);
        fprintf(2, "exec failed\n");
        exit(1);
    } else {
        //parent process
        int status;
        wait(&status);

        // Stop timing of ticks
        int end_time = uptime();

        // Calculating elapsed time
        int elapsed_time = end_time - start_time;
        printf("Time: %d ticks\n", elapsed_time);
        printf("elapsed time: %d ticks\n", elapsed_time);
    }

    exit(0);
}
