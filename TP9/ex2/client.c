#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "structures.h"

int main(int argc, char *argv[]) {
    int sock, choice;
    struct sockaddr_in serverAddress;
    request_t request;

    if (argc < 4) {
        fprintf(stderr, "At least 3 arguments are required\n");
        exit(EXIT_FAILURE);
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("An error occurred while attempting to create the socket");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddress, 0, sizeof(struct sockaddr_in));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &serverAddress.sin_addr) != 1) {
        perror("An error occurred while attempting to convert the address");
        exit(EXIT_FAILURE);
    }

    request.type = TYPE_CONNECTION;

    if (sendto(sock, &request, sizeof(request_t), 0, (struct sockaddr *) &serverAddress, sizeof(struct sockaddr_in)) == -1) {
        perror("An error occurred while attempting to send the message");
        exit(EXIT_FAILURE);
    }

    do {
        printf("\n=== MENU ===\n\n");
        printf("=== 1 - List files\n");
        printf("=== 2 - Download file\n");
        printf("=== 3 - Upload file\n");
        printf("=== 4 - Delete file\n");
        printf("=== 0 - Quit\n\n");
        printf("Choice? ");

        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "An error occurred while waiting for keyboard input\n");
            exit(EXIT_FAILURE);
        }
    } while (choice != 0);

    if (close(sock) == -1) {
        perror("Erreur lors de la fermeture de la socket ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
