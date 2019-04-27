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
    struct sockaddr *clientAddress;
    queue_t *queue;
    connect_udp_master_t *request_connect;
    queue_element_t *qElem;
    send_address_udp_t response_connect;
    struct sigaction action;
    ssize_t bytesReceived;
    bool sentMessage;

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

    /*
     * Determines how many clients can be in the queue before we stop accepting connections
     * Doesn't mean it's the maximum number of clients as requests are popped out of the queue when we connect two of them together
     */
    queue = createQueue(QUEUE_SIZE);

    /* FIFO queue, first client connected will be the first "served" */
    while (run) {
        request_connect = (connect_udp_master_t*) malloc(sizeof(connect_udp_master_t));
        clientAddress = (struct sockaddr*) malloc(sizeof(struct sockaddr));
        memset(clientAddress, 0, sizeof(struct sockaddr));
        clientAddressLength = sizeof(struct sockaddr_in);

        /* Receive in the biggest struct of the two, depends on the fact that the first variable is the same for both structs  */
        bytesReceived = receiveUDP(sock, request_connect, sizeof(connect_udp_master_t), 0, clientAddress, &clientAddressLength);

        /* Master connected */
        if (bytesReceived == sizeof(connect_udp_master_t)) {
            printf("Master just connected (TCP Port: %d)\n", request_connect->port);

            /* If the front of the queue is a slave client then we send him the master's info */
            if (front(queue) != NULL && ((connect_udp_master_t*) front(queue)->request)->type == TYPE_CONNECT_UDP_SLAVE) {
                qElem = dequeue(queue);

                response_connect.port = request_connect->port;

                if (inet_ntop(AF_INET, &((struct sockaddr_in*) clientAddress)->sin_addr, response_connect.address, INET_ADDRSTRLEN) == NULL) {
                    exit(EXIT_FAILURE);
                }

                printf("Sending address and TCP port (%d) to the Slave client\n", response_connect.port);
                sendUDP(sock, &response_connect, sizeof(send_address_udp_t), qElem->sourceAddr, qElem->addrLen);
                sentMessage = TRUE;
            }

            else {
                sentMessage = FALSE;
            }
        }

        else if (bytesReceived == sizeof(connect_udp_slave_t)) {
            printf("Slave just connected\n");

            /* If the front of the queue is a master client then we dequeue and get his info */
            if (front(queue) != NULL && ((connect_udp_master_t*) front(queue)->request)->type == TYPE_CONNECT_UDP_MASTER) {
                qElem = dequeue(queue);

                response_connect.port = ((connect_udp_master_t*) (qElem->request))->port;

                if (inet_ntop(AF_INET, &((struct sockaddr_in*) qElem->sourceAddr)->sin_addr, response_connect.address, INET_ADDRSTRLEN) == NULL) {
                    exit(EXIT_FAILURE);
                }

                printf("Sending Address and TCP Port (%d) to the Slave client\n", response_connect.port);
                sendUDP(sock, &response_connect, sizeof(send_address_udp_t), clientAddress, clientAddressLength);
                sentMessage = TRUE;
            }

            else {
                sentMessage = FALSE;
            }
        }

        else {
            fprintf(stderr, "Error: Unknown structure received\n");
            exit(EXIT_FAILURE);
        }

        if (!sentMessage) {
            if (isFull(queue)) {
                fprintf(stderr, "Maximum number of clients in queue reached\n");
            }

            else {
                qElem = (queue_element_t *) malloc(sizeof(queue_element_t));
                qElem->request = request_connect;
                qElem->sourceAddr = clientAddress;
                qElem->addrLen = clientAddressLength;
                enqueue(queue, qElem);
            }
        }
    }

    destroyQueue(queue);
    printf("\tStopped server\n");

    return EXIT_SUCCESS;
}