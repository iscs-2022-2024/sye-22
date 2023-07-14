/*
 * Copyright (C) 2021 David Truan <david.truan@heig-vd.ch>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "socket_sye.h"

#define BUFFER_LEN  80

/* Variable de "routage" */
msg_idx_t message_to_send = 0;

/* A COMPLETER: Handlers des signaux */

void int_handler(int sig) {
    printf("Handler INT\n"); 
    message_to_send = QUITING;
    fflush(stdout);
}

void user_handler(int sig) {
    switch(sig) {
        case SIGUSR1:
            printf("Handler USR1\n");
            message_to_send = GREETING;
            break;
        case SIGUSR2:
            printf("Handler USR2\n");
            message_to_send = COUNTING;
            break;
    }
    fflush(stdout);
}

int main(int argc, char **argv) {
    /* A COMPLETER: Variables */
    int running = 1;
    struct sockaddr_in serv_addr;
    int sock_fd;
    int port;

    /* A COMPLETER: Traitement des arguments */
    if (argc != 3) {
        printf("Usage: %s <ip address> <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    port = atoi(argv[2]);

    /* A COMPLETER: Liens entre signaux et handlers */
    signal(SIGINT, int_handler);
    signal(SIGUSR1, user_handler);
    signal(SIGUSR2, user_handler);

    /* A COMPLETER: Création du socket et connection au serveur */

    // creation socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Failed to create socket\n");
        fflush(stdout);
        return EXIT_FAILURE;
    }

    // preparation a la connexion
    ZERO_MEMORY(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if ((inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)) <= 0) {
        printf("Failed to convert ip address format\n"); fflush(stdout);
        return EXIT_FAILURE;
    }

    // connexion

    if ((connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0) {
        printf("Connection failed\n"); fflush(stdout);
        return EXIT_FAILURE;
    } else {
        printf("Connecting to server at %s:%d... SUCCESS\n", argv[1], port); fflush(stdout);
    }

    int alt = 1; // variable pour alterner Bonjour / Aurevoir
    

    while (running) {
        /* buffer de reception */

        char buffer[BUFFER_LEN];

        /* A COMPLETER: Préparation en envoi du message */
        switch (message_to_send) {
            case GREETING:
                snprintf(buffer, BUFFER_LEN, alt ? BONJOUR : AUREVOIR);
                alt = !alt;
                break;
            case COUNTING:
                snprintf(buffer, BUFFER_LEN, COMPTEUR);
                break;
            case QUITING:
                snprintf(buffer, BUFFER_LEN, QUITTER);
                break;
            default: continue;
        }
        write(sock_fd, buffer, BUFFER_LEN);

        /* A COMPLETER: Réception, si nécessaire de la réponse */
        ZERO_MEMORY(buffer, sizeof(buffer));
        switch (message_to_send) {
            case QUITING:
                running = 0;
                break;
            case GREETING:
            case COUNTING:
            recv(sock_fd, buffer, BUFFER_LEN, 0);
        }
        printf("%s\n", buffer); fflush(stdout);

        message_to_send = 0;
    }

    /* A COMPLETER: Nettoyage des ressources */

    close(sock_fd);

    return 0;
}
