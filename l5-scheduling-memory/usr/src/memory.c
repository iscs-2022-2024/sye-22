#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

#define PAGE_SIZE 0x1000

int main(int argc, char **argv) {
    
    /* A COMPLETER */

    uint32_t *virt = (uint32_t *)malloc(PAGE_SIZE * sizeof(int32_t));
    if (!virt) {
        fprintf(stderr, "Failed to allocate page\n");
        return EXIT_FAILURE;
    }

    uint32_t *phy = sys_translate(virt);

    fprintf(stdout, "page 4KiB: virt: %p, phy: %p\n", virt, phy);

    free(virt);
    
    return EXIT_SUCCESS;
}
