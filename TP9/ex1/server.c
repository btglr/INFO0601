#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "structures.h"
#include "queue.h"

int main(int argc, char *argv[]) {
    int sock;
    uint16_t port;
    socklen_t clientAddressLength;
    struct sockaddr_in serverAddress;
    struct sockaddr clientAddress;
    message_t message;
    queue_t *queue;

    if (argc < 2) {
        fprintf(stderr, "At least 1 argument is required\n");
        exit(EXIT_FAILURE);
    }

    port = (uint16_t) atoi(argv[1]);

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("An error occurred while attempting to create the socket");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddress, 0, sizeof(struct sockaddr_in));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    memset(&clientAddress, 0, sizeof(struct sockaddr));

    if (bind(sock, (struct sockaddr*) &serverAddress, sizeof(struct sockaddr_in)) == -1) {
        perror("An error occurred while binding the socket");
        exit(EXIT_FAILURE);
    }

    queue = createQueue(10);

    while (1) {
        if (recvfrom(sock, &message, sizeof(message_t), 0, &clientAddress, &clientAddressLength) == -1) {
            perror("An error occurred while waiting to receive the message");
            exit(EXIT_FAILURE);
        }

        if (message.type == MESSAGE) {
            enqueue(queue, message.message);
            printf("Message: %s\n", message.message);
        }

        else {
            if (!isEmpty(queue)) {
                strncpy(message.message, dequeue(queue), 256);
                printf("Message to send: %s\n", message.message);

                if (sendto(sock, &message, sizeof(message_t), 0, &clientAddress, sizeof(struct sockaddr_in)) == -1) {
                    perror("An error occurred while sending the message");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    /*if (close(sock) == -1) {
        perror("An error occurred while closing the socket");
        exit(EXIT_FAILURE);
    }*/
}
