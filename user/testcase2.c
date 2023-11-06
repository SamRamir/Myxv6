#include "kernel/types.h"
#include "user/user.h"

int main() {
    int page_size = 4096;  // Page size in xv6 (4 KB)

    // Step 1: Allocate one page of memory
    char *mem = sbrk(page_size);
    if (mem == (char*)-1) {
        printf("Memory allocation failed\n");
        exit(0);
    }

    // Step 2: Check that memory was successfully allocated
    printf("Memory allocated at address: %p\n", mem);

    // Step 3: Write to the allocated memory
    mem[0] = 'A';
    printf("Wrote 'A' to the memory at address: %p\n", mem);

    // Step 4: Free the allocated memory
    sbrk(-page_size);
    printf("Memory freed\n");

    exit(0);
}
