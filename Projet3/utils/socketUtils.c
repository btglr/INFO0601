#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include "socketUtils.h"
#include "ncurses.h"

int createSocket(int type, int protocol) {
    int fd;

    if ((fd = socket(AF_INET, type, protocol)) == -1) {
        stop_ncurses();
        perror("An error occurred while creating the socket");
        exit(EXIT_FAILURE);
    }

    return fd;
}

void initAddressAny(struct sockaddr_in *address, int port) {
    memset(address, 0, sizeof(*address));
    address->sin_family = AF_INET;
    address->sin_port = htons((uint16_t) port);
    address->sin_addr.s_addr = htonl(INADDR_ANY);
}

void initAddress(struct sockaddr_in *address, int port, char* ipAddress) {
    memset(address, 0, sizeof(*address));
    address->sin_family = AF_INET;
    address->sin_port = htons((uint16_t) port);

    str_addr_to_network(AF_INET, ipAddress, &address->sin_addr);
}

ssize_t sendUDP(int sock, const void *msg, size_t msgLength, const void *destAddr, socklen_t addrLength) {
    ssize_t bytesSent;

    if ((bytesSent = sendto(sock, msg, msgLength, 0, (struct sockaddr*) destAddr, addrLength)) == -1) {
        stop_ncurses();
        perror("An error occurred while sending the message through the socket");
        exit(EXIT_FAILURE);
    }

    return bytesSent;
}

ssize_t receiveUDP(int sock, void *msg, size_t msgLength, int flags, void *srcAddr, socklen_t *addrLength) {
    ssize_t bytesReceived;

    if ((bytesReceived = recvfrom(sock, msg, msgLength, flags, (struct sockaddr*) srcAddr, addrLength)) == -1) {
        if (errno != EINTR) {
            stop_ncurses();
            perror("An error occurred while receiving the message via the socket");
            exit(EXIT_FAILURE);
        }
    }

    return bytesReceived;
}

void bindAddress(int sock, const void *addr) {
    /* Allows the socket to connect to a port that's in TIME_WAIT */
    /*int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));*/

    if (bind(sock, (struct sockaddr*) addr, sizeof(struct sockaddr_in)) == -1) {
        stop_ncurses();
        perror("An error occurred while binding the address to the socket");
        exit(EXIT_FAILURE);
    }
}

void listenSocket(int sock, int nbQueue) {
    if (listen(sock, nbQueue) == -1) {
        stop_ncurses();
        perror("An error occurred while marking the socket as a passive socket");
        exit(EXIT_FAILURE);
    }
}

int acceptSocket(int sock) {
    int newSock;

    if ((newSock = accept(sock, NULL, NULL)) == -1) {
        stop_ncurses();
        perror("An error occurred while creating a client socket");
        exit(EXIT_FAILURE);
    }

    return newSock;
}

void connectSocket(int sock, const void *addr) {
    if (connect(sock, (struct sockaddr*) addr, sizeof(struct sockaddr_in)) == -1) {
        stop_ncurses();
        perror("An error occurred while connecting to an established socket");
        exit(EXIT_FAILURE);
    }
}

void str_addr_to_network(int family, const char *addr, void *addrBuffer) {
    if (inet_pton(family, addr, addrBuffer) <= 0) {
        stop_ncurses();
        fprintf(stderr, "An error occurred while converting the string address to a network address\n");
        exit(EXIT_FAILURE);
    }
}

void network_addr_to_str(int family, const void *addr, char *addrBuffer, socklen_t len) {
    if (inet_ntop(family, addr, addrBuffer, len) == NULL) {
        stop_ncurses();
        fprintf(stderr, "An error occurred while converting the network address to a string address\n");
        exit(EXIT_FAILURE);
    }
}
