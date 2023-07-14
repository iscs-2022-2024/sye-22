/**
 * @file    tictactoe_player.c
 * @author  REDS Institute / HEIG-VD
 * @date    2021-10-04
 *
 * @brief   SYE 2021 - Lab02
 *
 * Tic-tac-toe player.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <tictactoe.h>

/**
 * @brief   Parse a string with the index of the cell to select.
 *
 * @param str   String to parse
 * @param index Index of the cell
 *
 * @returns 1 if the string format is correct, 0 otherwise.
 */
static int parse_line(char *str, int *index) {
    int  i = 0;                           // Global counter
    int  j = 0;                           // Local counter in the buffer
    char buffer[MAX_BUFFER_LENGTH + 1];   // Buffer

    if (str[0] == '\0') {
        return 0;
    }

    // First part of the string: try to read the index of the pile
    while (i < MAX_BUFFER_LENGTH) {
        // If this is a number...
        if ((str[i] >= '0') && (str[i] <= '9')) {
            buffer[j++] = str[i];
            i++;
        }
        // If this is the end of the line...
        else if (str[i] == '\0') {
            buffer[j] = '\0';
            // Convert the first part of the string to integer
            *index = atoi(buffer);

            if ((*index <= 0) || (*index > 9)) {
                return 0;
            }

            (*index)--;
            return 1;
        }
        // Unwanted character
        else {
            return 0;
        }
    }

    return 0;
}

/**
 * @brief   Function to display prompt.
 */
static void displayTictactoePrompt(void) {
    printf("tictactoe%% ");
    fflush(stdout);
}

/**
 * @brief   Function to display winnner.
 */
static void displayWinner(void) {
    printf("YOU WIN! New game (Y/N)?\n");
    fflush(stdout);
}

/**
 * @brief   Function to display looser.
 */
static void displayLooser(void) {
    printf("YOU LOSE! New game (Y/N)?\n");
    fflush(stdout);
}

/**
 * @brief   Get move from terminal input.
 */
static int get_input_move(void) {
    // TO COMPLETE
    char s[MAX_BUFFER_LENGTH];
    int index;
    displayTictactoePrompt();
    scanf("%s", s);
    while (parse_line(s, &index) == 0) {
        printf("Invalid!\n");
        fflush(stdout);
        displayTictactoePrompt();
        scanf("%s", s);
    }

    return index;
}

/**
 * @brief   Main function.
 */
int main(int argc, char **argv) {
    int game_over = 0, cmd;
    int pending_move;

    if (argc != 3) {
        return EXIT_FAILURE;
    }

    ipc_init_child(argv);   // DON'T REMOVE

    while (game_over == 0) {
        cmd = player_get_cmd();

        switch (cmd) {
            case GAME_START:
                game_over = 0;
                break;

            case GAME_END:
                printf("Game ended unexpectedly, sorry\n");
                game_over = 1;
                break;

            case YOU_CAN_PLAY:
                pending_move = get_input_move();
                player_send_cmd(pending_move);
                break;

            case MOVE_REJECTED:
                printf("You cannot play here, try again.\n");
                pending_move = get_input_move();
                player_send_cmd(pending_move);

                break;

            case NO_WINNER:
                printf("Draw :s\n");
                game_over = 1;
                break;

            case YOU_WIN:
                displayWinner();

                game_over = 1;
                break;

            case YOU_LOSE:
                displayLooser();

                game_over = 1;
                break;

            default:
                break;
        }
    }

    return 0;
}
