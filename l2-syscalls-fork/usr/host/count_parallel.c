#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_CHILDS          15
#define ID_LENGTH           1
#define PARALLEL_MODE       'p'
#define SEQUENTIAL_MODE     's'

#define XSTR(s) STR(s)
#define STR(s) #s

int main(int argc, char **argv) {
    int n;
    char id[ID_LENGTH + 1];
    char mode;
    pid_t pid;
    pid_t pids[MAX_CHILDS] = { 0 };
    size_t i;

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <n> <mode>\n", argv[0]);
        return EXIT_FAILURE;
    }

    n = atoi(argv[1]);
    mode = argv[2][0];

    if (mode != PARALLEL_MODE && mode != SEQUENTIAL_MODE) {
        fprintf(stderr, "You must choose either " XSTR(PARALLEL_MODE) " or " XSTR(SEQUENTIAL_MODE) " for the mode\n");
        return EXIT_FAILURE;
    }

    if (n > MAX_CHILDS) {
        fprintf(stderr, "Number of childs must be less or equal to %d\n", MAX_CHILDS);
        return EXIT_FAILURE;
    }

    for (i = 0; i < n; ++i) {
        sprintf(id, "%zu", i);
        pid = fork();
        pids[i] = pid;
        if (pid == 0) { // handling child process
            execl("./count.elf", "count", id, (char *) NULL);
        } else if (pid > 0 && mode == SEQUENTIAL_MODE) { 
            waitpid(pid, NULL, 0);
        } else if (pid < 0) {
            fprintf(stderr, "Failed to fork child process!\n"); 
            return EXIT_FAILURE;
        }
    }
    
    // freeing memory
    if (mode == PARALLEL_MODE) {
        for (i = 0; i < n; ++i) {
            if (pids[i] && pids[i] > 0) {
                waitpid(pids[i], NULL, 0);
            }
        }
    }
   

    return EXIT_SUCCESS;
}

