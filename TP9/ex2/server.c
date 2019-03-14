#include <stdint.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include "queue.h"
#include "structures.h"

int run = 1;

void handler(int sig) {
    if (sig == SIGINT) {
        run = 0;
    }
}

int main(int argc, char *argv[]) {
    int sock, tmpSock = -1;
    uint16_t port;
    socklen_t clientAddressLength;
    struct sockaddr_in connectAddress, requestAddress;
    struct sockaddr clientAddress;
    struct sigaction action;
    request_t request;
    response_t response;
    queue_t *queue;

    if (argc < 2) {
        fprintf(stderr, "At least 1 argument is required\n");
        exit(EXIT_FAILURE);
    }

    sigemptyset(&action.sa_mask);
    action.sa_handler = handler;

    if (sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "An error occurred while catching the SIGINT signal\n");
        exit(EXIT_FAILURE);
    }

    port = (uint16_t) atoi(argv[1]);

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("An error occurred while attempting to create the socket");
        exit(EXIT_FAILURE);
    }

    memset(&connectAddress, 0, sizeof(struct sockaddr_in));
    connectAddress.sin_family = AF_INET;
    connectAddress.sin_port = htons(port);
    connectAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    memset(&clientAddress, 0, sizeof(struct sockaddr));

    if (bind(sock, (struct sockaddr*) &connectAddress, sizeof(struct sockaddr_in)) == -1) {
        perror("An error occurred while binding the socket");
        exit(EXIT_FAILURE);
    }

    queue = createQueue(10);

    while (run) {
        printf("Waiting for packet\n");

        if (recvfrom(sock, &request, sizeof(request_t), 0, &clientAddress, &clientAddressLength) == -1) {
            perror("An error occurred while waiting to receive the message");
            exit(EXIT_FAILURE);
        }

        switch (request.type) {
            case TYPE_CONNECTION:
                printf("Received connection request\n");

                if ((tmpSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
                    perror("An error occurred while attempting to create the socket");
                    exit(EXIT_FAILURE);
                }

                memset(&requestAddress, 0, sizeof(struct sockaddr_in));
                requestAddress.sin_family = AF_INET;
                requestAddress.sin_addr.s_addr = htonl(INADDR_ANY);
                requestAddress.sin_port = 0;

                if (bind(tmpSock, (struct sockaddr*) &requestAddress, sizeof(struct sockaddr_in)) == -1) {
                    if (errno == EADDRINUSE) {
                        fprintf(stderr, "Address already in use\n");
                    }

                    perror("An error occurred while attempting to bind the socket");
                    exit(EXIT_FAILURE);
                }

                break;

            case TYPE_LIST:
                if (tmpSock) {
                    response.response = "Lol";

                    enqueue(queue, "ta mere la werror");

                    if (sendto(tmpSock, &response, sizeof(response_t), 0, &clientAddress, sizeof(struct sockaddr_in)) == -1) {
                        perror("An error occurred while attempting to send the message");
                        exit(EXIT_FAILURE);
                    }
                }

                break;

            case TYPE_DOWNLOAD:
                break;

            case TYPE_UPLOAD:
                break;

            case TYPE_DELETE:
                break;
        }
    }

    if (close(sock) == -1) {
        perror("An error occurred while closing the socket");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}

