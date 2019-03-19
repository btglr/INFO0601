#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "socketUtils.h"
#include "structures.h"

int main(int argc, char *argv[]) {
    unsigned char clientType;
    char serverIpAddress[16], filename[MAX_FILENAME_LENGTH];
    int serverPort, sock;
    unsigned short tcpPort;
    struct sockaddr_in serverAddress;
    request_connect_t request_connect;

    /*
     * argv[0] -> unused
     * argv[1] = M or S
     * argv[2] = Server IP address
     * argv[3] = Server Port
     * argv[4] = TCP Port for the SLAVE client
     * argv[5] = Map name
     */

    if (!(argc >= 6 || argc == 4)) {
        fprintf(stderr, "Usage: ./client.out TYPE SERVER_IP SERVER_PORT [CLIENT_PORT] [MAP]\n\n");
        fprintf(stderr, "TYPE:\t\tEither M[aster] or S[lave]\n");
        fprintf(stderr, "SERVER_IP:\tFor example, 127.0.0.1\n");
        fprintf(stderr, "SERVER_PORT:\tFor example, 9999\n");
        fprintf(stderr, "CLIENT_PORT:\tOnly in M[aster] mode: A TCP port for a client connection\n");
        fprintf(stderr, "MAP:\t\tOnly in M[aster] mode: map_1.bin\n");
        exit(EXIT_FAILURE);
    }

    clientType = (unsigned char) argv[1][0];
    serverPort = atoi(argv[3]);

    if (snprintf(serverIpAddress, sizeof(serverIpAddress), "%s", argv[2]) < 0) {
        fprintf(stderr, "An error occurred while formatting the server IP address\n");
        exit(EXIT_FAILURE);
    }

    if (argc >= 6) {
        tcpPort = (unsigned short) atoi(argv[4]);

        if (snprintf(filename, MAX_FILENAME_LENGTH, "%s", argv[5]) < 0) {
            fprintf(stderr, "An error occurred while formatting the map's filename\n");
            exit(EXIT_FAILURE);
        }

        printf("Filename: %s\n", filename);
        printf("TCP port: %d\n", tcpPort);
    }

    printf("Client type: %c\n", clientType);
    printf("IP Address: %s\n", serverIpAddress);
    printf("Server port: %d\n", serverPort);

    sock = createSocket(SOCK_DGRAM, IPPROTO_UDP);
    initAddress(&serverAddress, serverPort, serverIpAddress);

    if (argc >= 6) {
        request_connect.request.connect_udp_master.type = TYPE_CONNECT_UDP_MASTER;
        request_connect.request.connect_udp_master.port = tcpPort;
        request_connect.type = 255;

        sendto(sock, &request_connect, sizeof(request_connect_t), 0, (struct sockaddr*) &serverAddress, sizeof(struct sockaddr_in));
    }

    return EXIT_SUCCESS;
}