#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "queue.h"
#include "socketUtils.h"
#include "structures.h"

int main(int argc, char *argv[]) {
    int sock, port;
    socklen_t clientAddressLength;
    struct sockaddr_in serverAddress;
    struct sockaddr clientAddress;
    queue_t *queue;
    request_connect_t request_connect;

    if (argc < 2) {
        fprintf(stderr, "At least 1 argument is required\n");
        exit(EXIT_FAILURE);
    }

    port = atoi(argv[1]);
    sock = createSocket(SOCK_DGRAM, IPPROTO_UDP);

    initAddressAny(&serverAddress, port);

    memset(&clientAddress, 0, sizeof(struct sockaddr));

    if (bind(sock, (struct sockaddr*) &serverAddress, sizeof(struct sockaddr_in)) == -1) {
        perror("An error occurred while binding the socket");
        exit(EXIT_FAILURE);
    }

    queue = createQueue(10);

    while (1) {
        if (recvfrom(sock, &request_connect, sizeof(request_connect_t), 0, &clientAddress, &clientAddressLength) == -1) {
            perror("An error occurred while waiting to receive the message");
            exit(EXIT_FAILURE);
        }

        if (request_connect.type == TYPE_CONNECT_UDP_MASTER) {

        }

        else if (request_connect.type == TYPE_CONNECT_UDP_SLAVE) {

        }

        printf("Received request\n");
        enqueue(queue, "Lol");
    }
}