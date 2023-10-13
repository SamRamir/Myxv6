#include "kernel/types.h"
#include "user/user.h"

int main() {
  int pid;

  // Create child processes with different priorities
  for (int i = 0; i < 5; i++) {
    pid = fork();
    if (pid == 0) {
      // Child process
      setpriority(i * 10);  // Set priority (0, 10, 20, 30, 40)
      sleep(10);  // Sleep to allow other processes to run
      exit(0);
    }
  }

  // Parent process
  while (wait(0) != -1);

  exit(0);
}
