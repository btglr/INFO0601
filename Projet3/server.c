#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <curses.h>
#include "queue.h"
#include "socketUtils.h"
#include "structures.h"

bool run = TRUE;

void handler(int sig) {
    if (sig == SIGINT) {
        run = FALSE;
    }
}

int main(int argc, char *argv[]) {
    int sock, port;
    socklen_t clientAddressLength;
    struct sockaddr_in serverAddress;
    struct sockaddr clientAddress;
    queue_t *queue;
    request_connect_t request_connect;
    connect_udp_master_t masterConnect;
    connect_udp_slave_t slaveConnect;
    queue_element_t *qElem, *masterClient, *slaveClient;
    send_address_udp_t response_connect;
    struct sigaction action;
    ssize_t bytesReceived;

    if (argc < 2) {
        fprintf(stderr, "At least 1 argument is required\n");
        exit(EXIT_FAILURE);
    }

    port = atoi(argv[1]);
    sock = createSocket(SOCK_DGRAM, IPPROTO_UDP);

    initAddressAny(&serverAddress, port);
    bindAddress(sock, &serverAddress);

    sigemptyset(&action.sa_mask);
    action.sa_handler = handler;

    if (sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "An error occurred while catching the SIGINT signal\n");
        exit(EXIT_FAILURE);
    }

    queue = createQueue(QUEUE_SIZE);
    memset(&clientAddress, 0, sizeof(struct sockaddr));

    while (run) {
        clientAddressLength = sizeof(struct sockaddr_in);
        bytesReceived = receiveUDP(sock, &request_connect, sizeof(request_connect_t), &clientAddress, &clientAddressLength);

        if (bytesReceived != -1) {
            if (request_connect.type == TYPE_CONNECT_UDP_SLAVE || request_connect.type == TYPE_CONNECT_UDP_MASTER) {
                qElem = (queue_element_t *) malloc(sizeof(queue_element_t));
                qElem->request = request_connect;
                qElem->sourceAddr = clientAddress;
                qElem->addrLen = clientAddressLength;

                enqueue(queue, qElem);
            }

            /* If two clients are connected, dequeue */
            if (queue->size == QUEUE_SIZE) {
                if (front(queue)->request.type == TYPE_CONNECT_UDP_MASTER && rear(queue)->request.type == TYPE_CONNECT_UDP_SLAVE) {
                    masterClient = dequeue(queue);
                    slaveClient = dequeue(queue);
                }

                else if (front(queue)->request.type == TYPE_CONNECT_UDP_SLAVE && rear(queue)->request.type == TYPE_CONNECT_UDP_MASTER) {
                    slaveClient = dequeue(queue);
                    masterClient = dequeue(queue);
                }

                else {
                    fprintf(stderr, "The server requires a Master client and a Slave client");
                    exit(EXIT_FAILURE);
                }

                response_connect.port = masterClient->request.request.master.port;

                if (inet_ntop(AF_INET, &((struct sockaddr_in*) &masterClient->sourceAddr)->sin_addr, response_connect.address, INET_ADDRSTRLEN) == NULL) {
                    exit(EXIT_FAILURE);
                }

                printf("Sending address and TCP port to the Slave client\n");
                sendUDP(sock, &response_connect, sizeof(send_address_udp_t), &slaveClient->sourceAddr, slaveClient->addrLen);

                free(slaveClient);
                free(masterClient);
            }
        }
    }

    printf("\tStopped server\n");

    return EXIT_SUCCESS;
}