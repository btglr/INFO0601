#include <curses.h>
#include "utils/ncurses.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "structures/constants.h"
#include "utils/socketUtils.h"
#include "structures/structures.h"
#include "utils/memoryUtils.h"
#include "utils/fileUtils.h"
#include "utils/windowDrawer.h"
#include "utils/gameUtils.h"
#include "utils/chunkManager.h"
#include "structures/updateQueue.h"
#include "utils/threadUtils.h"
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>

WINDOW *gameWindow;
map_t *map;
updateQueue_t *updateQueue;
pthread_mutex_t displayMutex = PTHREAD_MUTEX_INITIALIZER;

bool run = TRUE;

void handler(int sig) {
    if (sig == SIGINT) {
        run = FALSE;
    }
}

int main(int argc, char *argv[]) {
    WINDOW *borderInformationWindow, *informationWindow, *borderGameWindow, *borderStateWindow, *stateWindow;
    chunk_size_t chunkSize;
    int currentChunk;
    int nbChunks;
    int i;
    int lemmingId = -1;
    bool isLemmingSelected;
    int mouseX;
    int relativeMouseX;
    int mouseY;
    int tool = -1;
    unsigned char nbLemmings = 0;
    char serverIpAddress[16];
    char path[MAX_PATH_LENGTH];
    char buffer[512];
    int serverPort;
    int sockUDP;
    int sockTCP = -1;
    int sockSlaveClient = -1;
    int ch;
    int event;
    unsigned short tcpPort;
    struct sockaddr_in serverUDPAddress;
    struct sockaddr_in serverTCPAddress;
    char *connection_response;
    int isMaster;
    int mapFd;
    char *connection_request;
    char *mapBuffer;
    char *request;
    unsigned char mapWidth;
    unsigned char mapHeight;
    size_t dataLength;
    unsigned short port;
    char address[16];
    struct sigaction action;
    unsigned char type;
    /*pthread_t *lemmings[NUMBER_LEMMINGS];*/
    lemming_t lemmings[NUMBER_LEMMINGS];
    coord_t *coords;
    int chunkPos;
    int typeOnSquare;
    lemming_t *currLemmingPtr;
    bool placed;

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

    fprintf(stderr, "DEBUG | Server IP Address: %s\n", serverIpAddress);
    fprintf(stderr, "DEBUG | Server Port: %d\n", serverPort);

    if (isMaster) {
        tcpPort = (unsigned short) atoi(argv[3]);

        if (snprintf(path, MAX_PATH_LENGTH, "%s", argv[4]) < 0) {
            fprintf(stderr, "An error occurred while formatting the map's filename\n");
            exit(EXIT_FAILURE);
        }

        fprintf(stderr, "DEBUG | Filename: %s\n", path);
        fprintf(stderr, "DEBUG | TCP port: %d\n", tcpPort);

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
    }

    else {
        dataLength = sizeof(unsigned char);
        connection_request = (char*) malloc_check(dataLength);

        connection_request[0] = TYPE_CONNECT_UDP_SLAVE;
    }

    sendUDP(sockUDP, connection_request, dataLength, &serverUDPAddress, sizeof(struct sockaddr_in));
    free(connection_request);

    fprintf(stderr, "DEBUG | Sent connection request to server\n");

    if (isMaster && sockTCP != -1) {
        /* Client is the master, wait for TCP connection from the slave */

        listenSocket(sockTCP, 1);
        sockSlaveClient = acceptSocket(sockTCP);

        /* TODO maybe RDWR */

        /* Open map file and copy it into variables */
        mapFd = openFile(path, O_RDONLY);
        readFileOff(mapFd, &mapWidth, 0, SEEK_SET, sizeof(unsigned char));
        readFile(mapFd, &mapHeight, sizeof(unsigned char));

        mapBuffer = (char*) malloc_check(mapWidth * mapHeight * sizeof(unsigned char));
        readFile(mapFd, mapBuffer, mapWidth * mapHeight * sizeof(unsigned char));

        dataLength = sizeof(unsigned char) * 3 + mapWidth * mapHeight * sizeof(unsigned char);
        request = (char*) malloc_check(dataLength);

        /* Preparing the request to send to the slave client */
        request[0] = TYPE_SEND_MAP;
        memcpy(request + sizeof(unsigned char), &mapWidth, sizeof(unsigned char));
        memcpy(request + 2 * sizeof(unsigned char), &mapHeight, sizeof(unsigned char));
        memcpy(request + 3 * sizeof(unsigned char), mapBuffer, mapWidth * mapHeight * sizeof(unsigned char));

        writeFile(sockSlaveClient, request, dataLength);
        free(request);

        fprintf(stderr, "DEBUG | Sent map to Slave Client (%ld bytes)\n", dataLength);

        readFile(sockSlaveClient, &type, sizeof(unsigned char));

        if (type == TYPE_RESPONSE_MAP) {
            fprintf(stderr, "DEBUG | Received OK from Slave Client\n");
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

        fprintf(stderr, "DEBUG | Received response from server, TCP Port: %d | Address: %s\n", port, address);

        sockTCP = createSocket(SOCK_STREAM, IPPROTO_TCP);
        initAddress(&serverTCPAddress, port, address);
        connectSocket(sockTCP, &serverTCPAddress);

        /* Now that the TCP socket is connected, read from it */
        readFile(sockTCP, &type, sizeof(unsigned char));

        if (type == TYPE_SEND_MAP) {
            type = TYPE_RESPONSE_MAP;

            readFile(sockTCP, &mapWidth, sizeof(unsigned char));
            readFile(sockTCP, &mapHeight, sizeof(unsigned char));
            mapBuffer = (char*) malloc_check(mapWidth * mapHeight * sizeof(unsigned char));

            if (readFile(sockTCP, mapBuffer, mapWidth * mapHeight * sizeof(unsigned char)) == mapWidth * mapHeight * sizeof(unsigned char)) {
                fprintf(stderr, "DEBUG | Received map from Master Client\nDimensions: %dw, %dh\n", mapWidth, mapHeight);

                writeFile(sockTCP, &type, sizeof(unsigned char));
                fprintf(stderr, "DEBUG | Sent OK to Master Client\n");
            }

            else {
                fprintf(stderr, "Error: expected more bytes than received\n");
                exit(EXIT_FAILURE);
            }

            fprintf(stderr, "DEBUG | Read %ld bytes\n", mapWidth * mapHeight *sizeof(unsigned char) + 3);
        }

        else {
            fprintf(stderr, "Expected request type %d, received type %d\n", TYPE_SEND_MAP, type);
            exit(EXIT_FAILURE);
        }

        /*while ((bytesRead = readFile(sockTCP, chunk, sizeof(unsigned char) * CHUNK_SIZE)) > 0) {
            fprintf(stderr, "DEBUG | Read %ld bytes\n", bytesRead);
            request = realloc(request, (size_t) totalRead + bytesRead);
            memcpy(request + totalRead, chunk, (size_t) bytesRead);
            totalRead += bytesRead;
            memset(chunk, 0, CHUNK_SIZE);
        }*/
    }

    /* Maximum of 256 updates in the queue */
    updateQueue = createUpdateQueue(256);

    /* Initializing ncurses */
    initialize_ncurses();
    ncurses_mouse();
    ncurses_colors();

    clear();
    refresh();

    init_pair(PAIR_COLOR_OBSTACLE, COLOR_EMPTY, COLOR_OBSTACLE);
    init_pair(PAIR_COLOR_EMPTY, COLOR_EMPTY, COLOR_EMPTY);
    init_pair(PAIR_COLOR_LEMMING, COLOR_WHITE, COLOR_LEMMING);
    init_pair(PAIR_COLOR_LEGEND, COLOR_RED, COLOR_BLACK);

    initializeGame(mapWidth, mapHeight, mapBuffer, &borderInformationWindow, &informationWindow, &borderGameWindow, &gameWindow, &borderStateWindow, &stateWindow);

    /* Determines the ideal chunk size according to the map's proportions */
    chunkSize = determineChunkSize(mapWidth, mapHeight);
    nbChunks = (mapHeight / chunkSize.height) * (mapWidth / chunkSize.width);

    map = (map_t*) malloc_check(sizeof(map_t));
    map->nbChunks = nbChunks;
    map->chunks = createChunks(mapWidth, mapHeight, chunkSize);
    map->width = mapWidth;
    map->height = mapHeight;
    map->chunkSize = chunkSize;

    initializeLemmings(lemmings, NUMBER_LEMMINGS);
    populateChunks(mapBuffer, map);

    printInformation(informationWindow, "Map (%d, %d) | Chunk width: %d, height: %d", mapWidth, mapHeight, chunkSize.width, chunkSize.height);

    while (run && (ch = getch()) != KEY_F(2)) {
        if ((ch == KEY_MOUSE) && (mouse_getpos(&mouseX, &mouseY, &event) == OK)) {
            if(wmouse_trafo(stateWindow, &mouseY, &mouseX, FALSE) != FALSE) {
                if (mouseY == COMMAND_TOOLS_LEMMINGS) {
                    i = 0;
                    isLemmingSelected = FALSE;

                    while (i < NUMBER_LEMMINGS && !isLemmingSelected) {
                        if (mouseX == COMMAND_TOOLS_POS_X + (i * 3) || mouseX == COMMAND_TOOLS_POS_X + (i * 3) + 1) {
                            lemmingId = i;
                            isLemmingSelected = TRUE;
                        }
                        
                        i++;
                    }

                    if (isLemmingSelected) {
                        printInformation(informationWindow, "Selected the lemming n°%d", lemmingId + 1);
                    }
                }
                
                else {
                    tool = getTool(mouseX, mouseY);
                    printInformation(informationWindow, "Selected tool %d", tool);
                }
            }

            else if(wmouse_trafo(gameWindow, &mouseY, &mouseX, FALSE) != FALSE) {
                relativeMouseX = mouseX / SQUARE_WIDTH;

                coords = (coord_t*) malloc_check(sizeof(coord_t));
                coords->x = relativeMouseX;
                coords->y = mouseY;

                currentChunk = getChunk(coords, map);

                printInformation(informationWindow, "Click (%d, %d) was in chunk %d", relativeMouseX, mouseY, currentChunk);

                chunkPos = globalToLocalCoordinate(coords, chunkSize);

                mutex_lock_check(&map->chunks[currentChunk].mutex);

                typeOnSquare = map->chunks[currentChunk].squares[chunkPos].type;

                /*fprintf(stderr, "DEBUG | Clicked on (%d, %d) in the chunk %d at local coordinate %d\n", coords->x, coords->y, currentChunk, chunkPos);*/

                mutex_unlock_check(&map->chunks[currentChunk].mutex);

                switch (tool) {
                    case TOOL_ADD:
                        if (nbLemmings <= NUMBER_LEMMINGS && lemmingId < NUMBER_LEMMINGS && typeOnSquare != OBSTACLE) {
                            mutex_lock_check(&map->chunks[currentChunk].mutex);

                            placed = placeLemming(gameWindow, &lemmings[lemmingId], map, makeMultipleOf(mouseX, SQUARE_WIDTH), mouseY);

                            mutex_unlock_check(&map->chunks[currentChunk].mutex);

                            if (placed) {
                                fprintf(stderr, "DEBUG | Lemming %d has been removed %d time(s)\n", lemmingId, lemmings[lemmingId].timesRemoved);
                                printInformation(informationWindow, "Placed lemming %d at (%d, %d)", lemmingId, relativeMouseX, mouseY);
                                updateToolbox(stateWindow, lemmingId, tool);
                            }
                        }

                        break;

                    case TOOL_REMOVE:
                        if (typeOnSquare == LEMMING) {
                            mutex_lock_check(&map->chunks[currentChunk].mutex);
                            lemmingId = map->chunks[currentChunk].squares[chunkPos].lemming->id;

                            if (map->chunks[currentChunk].squares[chunkPos].lemming->timesRemoved < 3) {
                                printInformation(informationWindow, "Removed lemming %d from the map", lemmingId);
                                removeLemming(gameWindow, &lemmings[lemmingId], map, currentChunk, chunkPos);
                                updateToolbox(stateWindow, lemmingId, tool);
                            }

                            else {
                                printInformation(informationWindow, "Lemming %d has already been removed the maximum amount of times", lemmingId);
                            }

                            mutex_unlock_check(&map->chunks[currentChunk].mutex);
                        }

                        break;

                    case TOOL_EXPLODE:
                        if (typeOnSquare == LEMMING) {
                            mutex_lock_check(&map->chunks[currentChunk].mutex);

                            currLemmingPtr = map->chunks[currentChunk].squares[chunkPos].lemming;
                            lemmingId = currLemmingPtr->id;

                            mutex_unlock_check(&map->chunks[currentChunk].mutex);

                            explodeLemming(gameWindow, &lemmings[lemmingId], map, currentChunk, chunkPos);

                            /* TODO Add it back to the list if it killed another lemming */
                            updateToolbox(stateWindow, lemmingId, TOOL_EXPLODE);
                        }

                        break;

                    case TOOL_FREEZE:
                        if (typeOnSquare == LEMMING) {
                            mutex_lock_check(&map->chunks[currentChunk].mutex);

                            currLemmingPtr = map->chunks[currentChunk].squares[chunkPos].lemming;
                            lemmingId = currLemmingPtr->id;

                            mutex_unlock_check(&map->chunks[currentChunk].mutex);

                            freezeLemming(&lemmings[lemmingId], map);

                            updateToolbox(stateWindow, lemmingId, TOOL_FREEZE);
                        }
                        break;

                    case TOOL_PAUSE_RESUME:
                        break;
                }

                free(coords);
            }
        }
    }

    delwin(informationWindow);
    delwin(borderInformationWindow);
    delwin(gameWindow);
    delwin(borderGameWindow);
    delwin(stateWindow);
    delwin(borderStateWindow);

    destroyUpdateQueue(updateQueue);
    free(map);
    free(mapBuffer);
    stop_ncurses();

    if (isMaster) {
        while (recv(sockSlaveClient, buffer, sizeof(buffer), 0) > 0) {}
        close(sockSlaveClient);
        close(sockTCP);
    }

    else {
        close(sockTCP);
    }

    return EXIT_SUCCESS;
}