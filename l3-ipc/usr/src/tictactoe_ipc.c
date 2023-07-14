#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "tictactoe_priv.h"

/* Private struct */
sye_ipc_t ipc;


/**
 * @brief Create the pipes and save the file descriptors in the ipc struct
 * @return 0;
 */
int ipc_init(void) {
    pipe(ipc.pipe1);    
    ipc.gm_recv = ipc.pipe1[READ_SIDE];
    ipc.player_send = ipc.pipe1[WRITE_SIDE];
    
    pipe(ipc.pipe2);
    ipc.player_recv = ipc.pipe2[READ_SIDE];
    ipc.gm_send = ipc.pipe2[WRITE_SIDE];
    return 0;
}

/**
 * @brief Initializes the player file descriptors
 * @param argv The 2 file descriptors for the child process
 */
int ipc_init_child(char **argv) {
    int fd1 = atoi(argv[1]);
    int fd2 = atoi(argv[2]);
    ipc.player_recv = fd1;
    ipc.player_send = fd2;
    return 0;
}

/**
 * @brief Closes all open pipes
 */
void ipc_close(void) {
    close(ipc.gm_recv);
    close(ipc.gm_send);
    close(ipc.player_recv);
    close(ipc.player_send);
}

/**
 * @brief Reads the pipe used to communicate from the player to the game manager
 * @return -1 if error, command value otherwise
 */
int gm_get_cmd(void) {
    int cmd;
    if (read(ipc.gm_recv, &cmd, CMD_SIZE) != -1) {
        return cmd;
    }
    return -1;
}


 /**
 * @brief Writes the pipe used to communicate from the the game manager to the player
 * @return -1 if error, 0 otherwise
 */
int gm_send_cmd(int cmd) {
    if (write(ipc.gm_send, &cmd, CMD_SIZE) != -1)
        return 0;
    return -1;
}

/**
 * @brief Reads the pipe used to communicate from the game manager to the player
 * @return -1 if error, command value otherwise
 */
int player_get_cmd(void) {
    int cmd;
    if (read(ipc.player_recv, &cmd, CMD_SIZE) != -1) {
        return cmd;
    }
    return -1;
}

/**
* @brief Writes the pipe used to communicate from the the player to the game manager
* @return -1 if error, 0 otherwise
*/
int player_send_cmd(int cmd) {
    if (write(ipc.player_send, &cmd, CMD_SIZE) != -1)
        return 0;
    return -1;
}

/**
 * @brief Get player receiving pipe file descriptor
 * @return Pipe file descriptor
 */
int ipc_player_argv1(void) {
    return ipc.player_recv;
}

/**
 * @brief Get player writing pipe file descriptor
 * @return Pipe file descriptor
 */
int ipc_player_argv2(void) {
    return ipc.player_send;
}
