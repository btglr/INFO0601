#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
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

    if (inet_pton(AF_INET, ipAddress, &address->sin_addr) != 1) {
        stop_ncurses();
        perror("An error occurred while converting the IP address");
        exit(EXIT_FAILURE);
    }
}
