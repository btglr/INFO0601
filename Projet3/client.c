#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "socketUtils.h"
#include "structures.h"
#include "memoryUtils.h"

int main(int argc, char *argv[]) {
    char serverIpAddress[16], filename[MAX_FILENAME_LENGTH];
    char *msg;
    int serverPort, sockUDP, sockTCP = -1, sockSlaveClient;
    unsigned short tcpPort;
    struct sockaddr_in serverUDPAddress, serverTCPAddress;
    char *connection_response;
    int isMaster;
    char *connection_request;
    size_t dataLength;
    unsigned short port;
    char address[16];
    /*int isSlave;*/

    /*
     * argv[0] -> unused
     * argv[1] = M or S
     * argv[2] = Server IP address
     * argv[3] = Server Port
     * argv[4] = TCP Port for the SLAVE client
     * argv[5] = Map name
     */

    if (!(argc >= 5 || argc == 3)) {
        fprintf(stderr, "Usage: ./client.out SERVER_IP SERVER_PORT [CLIENT_PORT] [MAP]\n\n");
        fprintf(stderr, "SERVER_IP:\tFor example, 127.0.0.1\n");
        fprintf(stderr, "SERVER_PORT:\tFor example, 9999\n");
        fprintf(stderr, "CLIENT_PORT:\tOnly in Master mode: A TCP port for a client connection\n");
        fprintf(stderr, "MAP:\t\tOnly in Master mode: map_1.bin\n");
        exit(EXIT_FAILURE);
    }

    isMaster = argc >= 5;
    /*isSlave = argc == 3;*/

    serverPort = atoi(argv[2]);

    if (snprintf(serverIpAddress, sizeof(serverIpAddress), "%s", argv[1]) < 0) {
        fprintf(stderr, "An error occurred while formatting the server IP address\n");
        exit(EXIT_FAILURE);
    }

    sockUDP = createSocket(SOCK_DGRAM, IPPROTO_UDP);
    initAddress(&serverUDPAddress, serverPort, serverIpAddress);

    printf("Server IP Address: %s\n", serverIpAddress);
    printf("Server Port: %d\n", serverPort);

    if (isMaster) {
        tcpPort = (unsigned short) atoi(argv[3]);

        if (snprintf(filename, MAX_FILENAME_LENGTH, "%s", argv[4]) < 0) {
            fprintf(stderr, "An error occurred while formatting the map's filename\n");
            exit(EXIT_FAILURE);
        }

        printf("Filename: %s\n", filename);
        printf("TCP port: %d\n", tcpPort);

        dataLength = sizeof(unsigned char) + sizeof(unsigned short);
        connection_request = (char*) malloc_check(dataLength);

        connection_request[0] = TYPE_CONNECT_UDP_MASTER;
        memcpy(connection_request + sizeof(unsigned char), &tcpPort, sizeof(unsigned short));

        sendUDP(sockUDP, connection_request, dataLength, &serverUDPAddress, sizeof(struct sockaddr_in));

        sockTCP = createSocket(SOCK_STREAM, IPPROTO_TCP);
        initAddress(&serverTCPAddress, tcpPort, serverIpAddress);
        bindAddress(sockTCP, &serverTCPAddress);
    }

    else {
        dataLength = sizeof(unsigned char);
        connection_request = (char*) malloc_check(dataLength);

        connection_request[0] = TYPE_CONNECT_UDP_SLAVE;

        sendUDP(sockUDP, connection_request, dataLength, &serverUDPAddress, sizeof(struct sockaddr_in));
    }

    if (isMaster && sockTCP != -1) {
        /* Client is the master, wait for TCP connection from the slave */

        listenSocket(sockTCP, 1);
        sockSlaveClient = acceptSocket(sockTCP);

        /* Can now read/write */

        if (read(sockSlaveClient, &msg, 5)) {

        }
    }

    else {
        /* Client is a slave, wait for message from server */

        dataLength = sizeof(char) * 16 + sizeof(unsigned short);
        connection_response = (char*) malloc_check(dataLength);

        receiveUDP(sockUDP, connection_response, dataLength, 0, NULL, NULL);

        memcpy(address, connection_response, sizeof(char) * 16);
        memcpy(&port, connection_response + sizeof(char) * 16, sizeof(unsigned short));

        printf("Received response from server, TCP Port: %d | Address: %s\n", port, address);

        sockTCP = createSocket(SOCK_STREAM, IPPROTO_TCP);
        initAddress(&serverTCPAddress, port, address);
        connectSocket(sockTCP, &serverTCPAddress);

        /* Can now read/write */

        if (write(sockTCP, "test", 5)) {

        }
    }

    return EXIT_SUCCESS;
}