/**
 * Ce programme permet d'envoyer un message au serveur via une socket UDP.
 * Le message, passé en argument du programme, est envoyé en deux temps :
 * sa taille, puis le contenu du message. L'adresse IP et le port du serveur
 * sont passés en argument du programme.
 * @author Cyril Rabat
 * @version 24/06/2015
 **/
#include <stdlib.h>      /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>       /* Pour printf, fprintf, perror */
#include <sys/socket.h>  /* Pour socket */
#include <arpa/inet.h>   /* Pour sockaddr_in, inet_pton */
#include <string.h>      /* Pour memset */
#include <unistd.h>      /* Pour close */
#include <errno.h>       /* Pour errno */
#include "structures.h"

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in serverAddress;
    message_t message;

    if (argc < 4) {
        fprintf(stderr, "At least 3 arguments are required\n");
        exit(EXIT_FAILURE);
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Erreur lors de la creation de la socket ");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddress, 0, sizeof(struct sockaddr_in));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &serverAddress.sin_addr) != 1) {
        perror("Erreur lors de la conversion de l'adresse ");
        exit(EXIT_FAILURE);
    }

    if (argc == 4) {
        message.type = REQUEST;

        if (sendto(sock, &message, sizeof(message_t), 0, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in)) == -1) {
            perror("Erreur lors de l'envoi de la length du message ");
            exit(EXIT_FAILURE);
        }

        if (recvfrom(sock, &message, sizeof(message_t), 0, NULL, NULL) == -1) {
            perror("An error occurred while waiting to receive the message");
            exit(EXIT_FAILURE);
        }

        printf("Received message %s\n", message.message);
    }

    else {
        message.type = MESSAGE;
        strncpy(message.message, argv[4], 255);
        message.message[255] = '\0';

        if (sendto(sock, &message, sizeof(message_t), 0, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in)) == -1) {
            perror("Erreur lors de l'envoi de la length du message ");
            exit(EXIT_FAILURE);
        }
    }

    if (close(sock) == -1) {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
