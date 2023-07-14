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
 #include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "socket_sye.h"

#define MAX_CONNECTIONS 5
#define PORT_NUMBER 5000
#define BUFFER_LEN 80

int main(int argc, char *argv[]) {

    /* A COMPLETER: déclarations des variables */
    int running = 1;
    int counter = 0;
    int listen_fd;
    int conn_fd;

    char buffer[BUFFER_LEN];
    ZERO_MEMORY(buffer, sizeof(buffer));

    /* A COMPLETER: Mise en place du socket et connexion avec le client */

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
   
    struct sockaddr_in serv_addr;
    ZERO_MEMORY(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT_NUMBER);

   
    bind(listen_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    listen(listen_fd, MAX_CONNECTIONS);

    /* Boucle de traitement principal. C'est ici que la reception des message 
        et l'envoi des réponses se fait. */
    printf("Waiting for clients...\n"); fflush(stdout);

    while (running) {
        /* A COMPLETER: Reception et traitement des messages */
        struct sockaddr_in client_addr;
        ZERO_MEMORY(&client_addr, sizeof(client_addr));

        char ip[INET_ADDRSTRLEN]; // buffer pour l'adresse ip du client
        size_t len = sizeof(client_addr);

        conn_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &len); 
        printf("Client connected!\n"); fflush(stdout);

        inet_ntop(AF_INET, &(client_addr.sin_addr.s_addr), ip, len);

        /* A COMPLETER: Envoi de la réponse */
        while ((read(conn_fd, buffer, BUFFER_LEN)) > 0) {
            printf("The client said: %s\n", buffer); fflush(stdout);
            if (strcmp(QUITTER, buffer) == 0) {
                printf("The client asked for a disconnection, now quitting...\n"); fflush(stdout);
                running = 0;
                break;
            }
            if (strcmp(BONJOUR, buffer) == 0) {
                ZERO_MEMORY(buffer, sizeof(buffer));
                snprintf(buffer, BUFFER_LEN, "Bonjour client %s", ip);
            } else if (strcmp(AUREVOIR, buffer) == 0) {
                ZERO_MEMORY(buffer, sizeof(buffer));
                snprintf(buffer, BUFFER_LEN, "Aurevoir client %s", ip);
            } else if (strcmp(COMPTEUR, buffer) == 0) {
                ZERO_MEMORY(buffer, sizeof(buffer));
                snprintf(buffer, BUFFER_LEN, "Valeur compteur: %d", ++counter);
            }
            write(conn_fd, buffer, BUFFER_LEN);
        }
        close(conn_fd);        
    }


    /* A COMPLETER: Nettoyage des ressources*/
    close(listen_fd);
    return 0;

}
