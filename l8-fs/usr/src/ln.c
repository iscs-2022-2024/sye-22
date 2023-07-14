#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <target> <link_name>\n", argv[0]);
        return -1;
    }
    char *target = argv[1];
    char *link_name = argv[2];
    int fd = open(target, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Failed to open target: %s\n", link_name);
        return -1;
    }
    if (sys_symlink(fd, link_name) < 0) {
        fprintf(stderr, "Failed to create symbolic link\n");
        if (close(fd) < 0) {
            fprintf(stderr, "Failed to close file descriptor!\n");
        }
        return -1;
    }
    if (close(fd) < 0) {
        fprintf(stderr, "Failed to close file descriptor!\n");
        return -1;
    }
    return 0;
}
