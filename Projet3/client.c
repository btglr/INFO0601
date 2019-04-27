#include <curses.h>
#include "ncurses.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "socketUtils.h"
#include "structures.h"
#include "memoryUtils.h"
#include "fileUtils.h"
#include "windowDrawer.h"
#include <fcntl.h>
#include <signal.h>

bool run = TRUE;

void handler(int sig) {
    if (sig == SIGINT) {
        run = FALSE;
    }
}

int main(int argc, char *argv[]) {
    WINDOW *borderInformationWindow, *informationWindow, *borderGameWindow, *gameWindow, *borderStateWindow, *stateWindow;
    char serverIpAddress[16];
    char path[MAX_PATH_LENGTH];
    int serverPort;
    int sockUDP;
    int sockTCP = -1;
    int sockSlaveClient;
    int ch;
    unsigned short tcpPort;
    struct sockaddr_in serverUDPAddress;
    struct sockaddr_in serverTCPAddress;
    char *connection_response;
    int isMaster;
    int mapFd;
    char *connection_request;
    char *map;
    char *request;
    unsigned char mapWidth;
    unsigned char mapHeight;
    size_t dataLength;
    unsigned short port;
    char address[16];
    struct sigaction action;
    /*char *response;
    ssize_t bytesRead;
    ssize_t totalRead = 0;
    unsigned char chunk[CHUNK_SIZE];*/
    unsigned char type;

    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    action.sa_handler = handler;

    if (sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "An error occurred while catching the SIGINT signal\n");
        exit(EXIT_FAILURE);
    }

    /*
     * argv[0] -> unused
     * argv[1] = Server IP address
     * argv[2] = Server Port
     * argv[3] = TCP Port for the SLAVE client
     * argv[4] = Map name
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

        if (snprintf(path, MAX_PATH_LENGTH, "%s", argv[4]) < 0) {
            fprintf(stderr, "An error occurred while formatting the map's filename\n");
            exit(EXIT_FAILURE);
        }

        printf("Filename: %s\n", path);
        printf("TCP port: %d\n", tcpPort);

        /* Establish the TCP socket before sending the connection request in case the port is taken, preventing the
         * server from queuing a client that won't be connected
         */
        sockTCP = createSocket(SOCK_STREAM, IPPROTO_TCP);
        initAddress(&serverTCPAddress, tcpPort, serverIpAddress);
        bindAddress(sockTCP, &serverTCPAddress);

        dataLength = sizeof(unsigned char) + sizeof(unsigned short);
        connection_request = (char*) malloc_check(dataLength);

        connection_request[0] = TYPE_CONNECT_UDP_MASTER;
        memcpy(connection_request + sizeof(unsigned char), &tcpPort, sizeof(unsigned short));

        sendUDP(sockUDP, connection_request, dataLength, &serverUDPAddress, sizeof(struct sockaddr_in));
        free(connection_request);

        printf("Sent connection request to server\n");
    }

    else {
        dataLength = sizeof(unsigned char);
        connection_request = (char*) malloc_check(dataLength);

        connection_request[0] = TYPE_CONNECT_UDP_SLAVE;

        sendUDP(sockUDP, connection_request, dataLength, &serverUDPAddress, sizeof(struct sockaddr_in));
        free(connection_request);

        printf("Sent connection request to server\n");
    }

    if (isMaster && sockTCP != -1) {
        /* Client is the master, wait for TCP connection from the slave */

        listenSocket(sockTCP, 1);
        sockSlaveClient = acceptSocket(sockTCP);

        /* TODO maybe RDWR */

        /* Open map file and copy it into variables */
        mapFd = openFile(path, O_RDONLY);
        readFileOff(mapFd, &mapWidth, 0, SEEK_SET, sizeof(unsigned char));
        readFile(mapFd, &mapHeight, sizeof(unsigned char));

        map = (char*) malloc_check(mapWidth * mapHeight * sizeof(unsigned char));
        readFile(mapFd, map, mapWidth * mapHeight * sizeof(unsigned char));

        dataLength = sizeof(unsigned char) * 3 + mapWidth * mapHeight * sizeof(unsigned char);
        request = (char*) malloc_check(dataLength);

        /* Preparing the request to send to the slave client */
        request[0] = TYPE_SEND_MAP;
        memcpy(request + sizeof(unsigned char), &mapWidth, sizeof(unsigned char));
        memcpy(request + 2 * sizeof(unsigned char), &mapHeight, sizeof(unsigned char));
        memcpy(request + 3 * sizeof(unsigned char), map, mapWidth * mapHeight * sizeof(unsigned char));

        writeFile(sockSlaveClient, request, dataLength);
        free(request);

        printf("Sent map to Slave Client\n");

        readFile(sockSlaveClient, &type, sizeof(unsigned char));

        if (type == TYPE_RESPONSE_MAP) {
            printf("Received OK from Slave Client\n");
        }

        else {
            fprintf(stderr, "Expected request type %d, received type %d\n", TYPE_RESPONSE_MAP, type);
            exit(EXIT_FAILURE);
        }
    }

    else {
        /* Client is a slave, wait for message from server */
        dataLength = sizeof(char) * 16 + sizeof(unsigned short);
        connection_response = (char*) malloc_check(dataLength);

        /* Receiving the master client's address and port from the server */
        receiveUDP(sockUDP, connection_response, dataLength, 0, NULL, NULL);

        memcpy(address, connection_response, sizeof(char) * 16);
        memcpy(&port, connection_response + sizeof(char) * 16, sizeof(unsigned short));

        free(connection_response);

        printf("Received response from server, TCP Port: %d | Address: %s\n", port, address);

        sockTCP = createSocket(SOCK_STREAM, IPPROTO_TCP);
        initAddress(&serverTCPAddress, port, address);
        connectSocket(sockTCP, &serverTCPAddress);

        /* Now that the TCP socket is connected, read from it */
        readFile(sockTCP, &type, sizeof(unsigned char));

        if (type == TYPE_SEND_MAP) {
            type = TYPE_RESPONSE_MAP;

            readFile(sockTCP, &mapWidth, sizeof(unsigned char));
            readFile(sockTCP, &mapHeight, sizeof(unsigned char));
            map = (char*) malloc_check(mapWidth * mapHeight * sizeof(unsigned char));

            if (readFile(sockTCP, map, mapWidth * mapHeight * sizeof(unsigned char)) == mapWidth * mapHeight * sizeof(unsigned char)) {
                printf("Received map from Master Client\nDimensions: %dw, %dh\n", mapWidth, mapHeight);

                writeFile(sockTCP, &type, sizeof(unsigned char));
                printf("Sent OK to Master Client\n");
            }

            else {
                fprintf(stderr, "Error: expected more bytes than received\n");
                exit(EXIT_FAILURE);
            }
        }

        else {
            fprintf(stderr, "Expected request type %d, received type %d\n", TYPE_SEND_MAP, type);
            exit(EXIT_FAILURE);
        }

        /*while ((bytesRead = readFile(sockTCP, chunk, sizeof(unsigned char) * CHUNK_SIZE)) > 0) {
            printf("Read %ld bytes\n", bytesRead);
            request = realloc(request, (size_t) totalRead + bytesRead);
            memcpy(request + totalRead, chunk, (size_t) bytesRead);
            totalRead += bytesRead;
            memset(chunk, 0, CHUNK_SIZE);
        }*/
    }

    /* Initializing ncurses */
    initialize_ncurses();
    ncurses_mouse();
    ncurses_colors();

    clear();
    refresh();

    init_pair(PAIR_COLOR_OBSTACLE, COLOR_EMPTY, COLOR_OBSTACLE);
    init_pair(PAIR_COLOR_EMPTY, COLOR_EMPTY, COLOR_EMPTY);

    borderInformationWindow = initializeWindow(
            mapWidth * SQUARE_WIDTH + BORDER_STATE_WINDOW_WIDTH + BORDER_WIDTH,
            BORDER_INFORMATION_WINDOW_HEIGHT,
            0,
            0);
    informationWindow = initializeSubWindow(borderInformationWindow,
            mapWidth * SQUARE_WIDTH + BORDER_STATE_WINDOW_WIDTH + BORDER_WIDTH - 2,
            BORDER_INFORMATION_WINDOW_HEIGHT - 2,
            1,
            1);
    borderGameWindow = initializeWindow(
            mapWidth * SQUARE_WIDTH + BORDER_WIDTH,
            mapHeight + BORDER_HEIGHT,
            0,
            BORDER_INFORMATION_WINDOW_HEIGHT);
    gameWindow = initializeSubWindow(borderGameWindow,
            mapWidth * SQUARE_WIDTH + BORDER_WIDTH - 2,
            mapHeight + BORDER_HEIGHT - 2,
            1,
            BORDER_INFORMATION_WINDOW_HEIGHT + 1);
    borderStateWindow = initializeWindow(
            BORDER_STATE_WINDOW_WIDTH,
            BORDER_STATE_WINDOW_HEIGHT,
            mapWidth * SQUARE_WIDTH + BORDER_WIDTH,
            BORDER_INFORMATION_WINDOW_HEIGHT);
    stateWindow = initializeSubWindow(borderStateWindow,
            BORDER_STATE_WINDOW_WIDTH - 2,
            BORDER_STATE_WINDOW_HEIGHT - 2,
            mapWidth * SQUARE_WIDTH + BORDER_WIDTH + 1,
            BORDER_INFORMATION_WINDOW_HEIGHT + 1);

    scrollok(informationWindow, true);

    box(borderInformationWindow, 0, 0);
    box(borderGameWindow, 0, 0);
    box(borderStateWindow, 0, 0);

    mvwprintw(borderInformationWindow, 0, 2, "Information");
    mvwprintw(borderGameWindow, 0, 2, "Map Editor");
    mvwprintw(borderStateWindow, 0, 2, "State");

    wrefresh(borderInformationWindow);
    wrefresh(informationWindow);
    wrefresh(borderGameWindow);
    wrefresh(gameWindow);
    wrefresh(borderStateWindow);
    wrefresh(stateWindow);

    drawMap(gameWindow, mapWidth, mapHeight, map);

    while (run && (ch = getch()) != KEY_F(2)) {
        printf("%d\n", ch);
    }

    delwin(informationWindow);
    delwin(borderInformationWindow);
    delwin(gameWindow);
    delwin(borderGameWindow);
    delwin(stateWindow);
    delwin(borderStateWindow);

    free(map);

    /* Stopping ncurses */
    stop_ncurses();

    return EXIT_SUCCESS;
}