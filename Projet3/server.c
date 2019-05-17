#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include <curses.h>
#include "structures/messageQueue.h"
#include "utils/socketUtils.h"
#include "structures/structures.h"
#include "utils/memoryUtils.h"

bool run = TRUE;

void handler(int sig) {
    if (sig == SIGUSR1) {
        run = FALSE;
    }
}

int main(int argc, char *argv[]) {
    int sock, port;
    socklen_t clientAddressLength;
    struct sockaddr_in serverAddress;
    struct sockaddr *clientAddress = NULL;
    messageQueue_t *queue;
    queue_element_t *qElem;
    char *connection_response;
    struct sigaction action;
    ssize_t bytesReceived;
    bool sentMessage;
    size_t dataLength;
    char *connection_request = NULL;
    unsigned char type;
    unsigned short masterPort;
    char strAddress[16];

    if (argc < 2) {
        fprintf(stderr, "At least 1 argument is required\n");
        exit(EXIT_FAILURE);
    }

    port = atoi(argv[1]);
    sock = createSocket(SOCK_DGRAM, IPPROTO_UDP);

    initAddressAny(&serverAddress, port);
    bindAddress(sock, &serverAddress);

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = handler;

    if (sigaction(SIGUSR1, &action, NULL) == -1) {
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
        clientAddress = (struct sockaddr*) malloc_check(sizeof(struct sockaddr));
        memset(clientAddress, 0, sizeof(struct sockaddr));
        clientAddressLength = sizeof(struct sockaddr_in);

        dataLength = sizeof(unsigned char) + sizeof(unsigned short);
        connection_request = (char*) malloc_check(dataLength);

        /* Receive in the biggest struct of the two, depends on the fact that the first variable is the same for both structs  */
        bytesReceived = receiveUDP(sock, connection_request, dataLength, 0, clientAddress, &clientAddressLength);

        if (bytesReceived != -1) {
            memcpy(&type, connection_request, sizeof(unsigned char));

            if (type == TYPE_CONNECT_UDP_MASTER || type == TYPE_CONNECT_UDP_SLAVE) {
                if (type == TYPE_CONNECT_UDP_MASTER) {
                    memcpy(&masterPort, connection_request + sizeof(unsigned char), sizeof(unsigned short));
                    fprintf(stderr, "DEBUG | Master just connected (TCP Port: %d)\n", masterPort);
                }

                else {
                    fprintf(stderr, "DEBUG | Slave just connected\n");
                }

                if (front(queue) != NULL) {
                    qElem = front(queue);
                    memcpy(&type, qElem->request, sizeof(unsigned char));
                    dequeue(queue);
                    dataLength = sizeof(char) * 16 + sizeof(unsigned short);
                    connection_response = (char*) malloc_check(dataLength);

                    if (type == TYPE_CONNECT_UDP_SLAVE) {
                        network_addr_to_str(AF_INET, &((struct sockaddr_in*) clientAddress)->sin_addr, strAddress, INET_ADDRSTRLEN);
                    }

                    else {
                        memcpy(&masterPort, qElem->request + sizeof(unsigned char), sizeof(unsigned short));
                        network_addr_to_str(AF_INET, &((struct sockaddr_in*) qElem->sourceAddr)->sin_addr, strAddress, INET_ADDRSTRLEN);
                    }

                    memcpy(connection_response, strAddress, sizeof(char) * 16);
                    memcpy(connection_response + sizeof(char) * 16, &masterPort, sizeof(unsigned short));
                    fprintf(stderr, "DEBUG | Sending address (%s) and TCP port (%d) to the Slave client\n", strAddress, masterPort);

                    if (type == TYPE_CONNECT_UDP_SLAVE) {
                        sendUDP(sock, connection_response, dataLength, qElem->sourceAddr, qElem->addrLen);
                    }

                    else {
                        sendUDP(sock, connection_response, dataLength, clientAddress, clientAddressLength);
                    }

                    sentMessage = TRUE;
                    free(connection_response);
                    free(qElem->sourceAddr);
                    free(qElem->request);
                    free(qElem);
                }

                else {
                    sentMessage = FALSE;
                }

                if (!sentMessage) {
                    if (isFull(queue)) {
                        fprintf(stderr, "Maximum number of clients in queue reached\n");
                    }

                    else {
                        qElem = (queue_element_t *) malloc_check(sizeof(queue_element_t));
                        qElem->request = connection_request;
                        qElem->sourceAddr = clientAddress;
                        qElem->addrLen = clientAddressLength;
                        enqueue(queue, qElem);
                    }
                }

                else {
                    free(clientAddress);
                    free(connection_request);
                }
            }

            else {
                fprintf(stderr, "Expected request type %d or %d, received type %d\n", TYPE_CONNECT_UDP_MASTER, TYPE_CONNECT_UDP_SLAVE, type);
                exit(EXIT_FAILURE);
            }
        }

        else {
            /* Probably received the SIGINT, break out of the loop so the frees will execute */
            break;
        }
    }

    /* If we get to this point then a SIGINT was received so the frees weren't done */

    free(clientAddress);
    free(connection_request);

    destroyQueue(queue);
    fprintf(stderr, "DEBUG | \tStopped server\n");

    return EXIT_SUCCESS;
}