#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf( "Usage: setpriority <priority>\n");
        exit(0);
    }

    int priority = atoi(argv[1]); // Convert the input to an integer
    if (priority < 0 || priority > 100) {
        printf( "Invalid priority value\n");
        exit(0);
    }

    // Call the sys_set_priority system call
    if (setpriority(priority) < 0) {
        printf( "Error setting priority\n");
        exit(0);
    }

    printf("Priority set to %d\n", priority);
    exit(0);
}
