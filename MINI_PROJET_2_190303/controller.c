#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "messageQueueUtils.h"
#include "semaphoreUtils.h"
#include "structures.h"
#include "fileUtils.h"
#include "sharedMemoryUtils.h"
#include "gameManager.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <wait.h>
#include <string.h>

bool run = TRUE;

void handler(int sig) {
    if (sig == SIGINT) {
        run = FALSE;
    }
}

int main(int argc, char *argv[]) {
    int i, j, queueId, semId, shmId, mapFd, keyQueue, keySem, keyShm, startingPos, nbMapSemaphores;
    char filename[MAX_FILENAME_LENGTH];
    unsigned short *values;
    struct sigaction action;
    request_t request;
    response_t response;
    map_t *map;
    pid_t minotaurs[MAX_MINOTAUR], player = -1;
    ssize_t result;

    if (argc == 5) {
        for (i = 0; argv[1][i] != '\0'; ++i) {
            filename[i] = argv[1][i];
        }

        filename[i] = '\0';
        keyQueue = atoi(argv[2]);
        keySem = atoi(argv[3]);
        keyShm = atoi(argv[4]);
    }

    else {
        printf("Please enter a filename to open: ");

        if (fgets(filename, MAX_FILENAME_LENGTH, stdin) == NULL) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }

        strcpy(filename, strtok(filename, "\n"));

        printf("Please enter the message queue, semaphore array and shared memory segment keys: ");

        if (scanf("%d %d %d", &keyQueue, &keySem, &keyShm) != 3) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < MAX_MINOTAUR; ++i) {
        minotaurs[i] = -1;
    }

    nbMapSemaphores = MAP_WIDTH / CHUNK_SIZE * MAP_HEIGHT / CHUNK_SIZE;

    printf("Required number of semaphores for chunks: %d\n", nbMapSemaphores);

    values = (unsigned short *) malloc((nbMapSemaphores + 2) * sizeof(unsigned short));
    values[SEM_MINOTAUR] = MAX_MINOTAUR;
    values[SEM_PLAYER] = 1;

    for (i = 0; i < nbMapSemaphores; ++i) {
        values[SEM_MAP_CHUNK + i] = 1;
    }

    sigemptyset(&action.sa_mask);
    action.sa_handler = handler;

    if (sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "An error occurred while catching the SIGINT signal\n");
        exit(EXIT_FAILURE);
    }

    mapFd = loadGame(filename);

    shmId = createSegment(keyShm, sizeof(map_t), S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL, false);

    if (shmId == -2) {
        printf("Shared memory segment already exists, opening\n");
        shmId = openSegment(keyShm);
    }

    map = (map_t*) attachSegment(shmId, 0);

    /* Write the map (loaded from file) into the shared memory segment */
    readFileOff(mapFd, &map->mapVersion, 0, SEEK_SET, sizeof(map->mapVersion));
    readFileOff(mapFd, &map->lives, 0, SEEK_CUR, sizeof(map->lives));
    map->livesLeft = map->lives;
    map->posX = X_COORDINATE_ENTRANCE;
    map->posY = Y_COORDINATE_ENTRANCE;
    readFileOff(mapFd, map->map, 0, SEEK_CUR, sizeof(map->map) * MAP_HEIGHT * MAP_WIDTH);

    startingPos = map->posY * MAP_WIDTH * sizeof(unsigned char) + map->posX * sizeof(unsigned char);
    map->map[startingPos] = PLAYER_SQUARE;

    /* Displays map, temporary */
    for (i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i) {
        printf("%d ", map->map[i]);

        if ((i + 1) % MAP_WIDTH == 0) {
            printf("\n");
        }
    }

    queueId = createMessageQueue(keyQueue, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL, false);

    if (queueId == -2) {
        printf("Queue already exists, opening\n");
        queueId = openMessageQueue(keyQueue);
    }


    semId = createSemaphores(keySem, nbMapSemaphores + 2, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL, false);

    if (semId == -2) {
        printf("Array of semaphores already exists, opening\n");
        semId = openSemaphores(keySem);
    }

    init(semId, values);

    i = 0;
    while (1) {
        if (!run) {
            break;
        }

        result = receiveMessage(queueId, &request, sizeof(request_t), RESPONSE_TYPE, MSG_EXCEPT);

        if (result > 0) {
            switch (request.type) {
                case GET_KEYS_TYPE:
                    printf("Received get keys request\n");

                    response.type = RESPONSE_TYPE;
                    response.semId = semId;
                    response.shmId = shmId;

                    sendMessage(queueId, &response, sizeof(response_t), 0);

                    break;

                case GAME_OVER_TYPE:
                    printf("Player has either lost or won, don't need the minotaurs anymore\n");

                    for (i = 0; i < MAX_MINOTAUR; ++i) {
                        if (minotaurs[i] != -1) {
                            kill(minotaurs[i], SIGINT);
                            receiveMessage(queueId, &request, sizeof(request_t), DISCONNECT_TYPE, 0);
                            minotaurs[i] = -1;
                        }
                    }

                    break;

                case CONNECT_TYPE:
                    printf("Received connect request from PID %d\n", request.message.connect.pid);

                    switch (request.message.connect.programType) {
                        case PLAYER:
                            player = request.message.connect.pid;
                            printf("Player %d just connected\n", player);

                            break;

                        case MINOTAUR:
                            minotaurs[i] = request.message.connect.pid;
                            printf("Minotaur %d just connected\n", minotaurs[i]);
                            i++;

                            break;
                    }

                    break;

                case DISCONNECT_TYPE:
                    switch (request.message.disconnect.programType) {
                        case PLAYER:
                            printf("Player %d just disconnected\n", player);

                            player = -1;

                            break;

                        case MINOTAUR:
                            for (j = 0; j < MAX_MINOTAUR && minotaurs[j] != request.message.disconnect.pid; ++j);

                            if (j <= MAX_MINOTAUR) {
                                printf("Minotaur %d just disconnected\n", minotaurs[j]);

                                /* If we found a corresponding pid, replace it by the last inserted pid */
                                i--;
                                minotaurs[j] = minotaurs[i];
                                minotaurs[i] = -1;
                            }

                            break;
                    }

                    break;
            }
        }
    }

    for (i = 0; i < MAX_MINOTAUR; ++i) {
        if (minotaurs[i] != -1) {
            kill(minotaurs[i], SIGINT);
            receiveMessage(queueId, &request, sizeof(request_t), DISCONNECT_TYPE, 0);
        }
    }

    if (player != -1) {
        kill(player, SIGINT);
        receiveMessage(queueId, &request, sizeof(request_t), DISCONNECT_TYPE, 0);
    }

    removeSegment(shmId);
    removeMessageQueue(queueId);
    removeSemaphores(semId);

    return EXIT_SUCCESS;
}
