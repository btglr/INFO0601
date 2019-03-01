#undef _GNU_SOURCE
#define _GNU_SOURCE

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

bool run = TRUE;

void handler(int sig) {
    if (sig == SIGINT) {
        run = FALSE;
    }
}

int main(int argc, char *argv[]) {
    int i, j, queueId, semId, shmId, mapFd;
    char filename[MAX_FILENAME_LENGTH];
    unsigned short values[3] = { MAX_MINOTAUR, 1, 1 };
    struct sigaction action;
    request_t request;
    response_t response;
    map_t *map;
    pid_t minotaurs[MAX_MINOTAUR], player = -1;
    ssize_t result;

    if (argc == 2) {
        for (i = 0; argv[1][i] != '\0'; ++i) {
            filename[i] = argv[1][i];
        }

        filename[i] = '\0';
    }

    else {
        printf("Please enter a filename to open: ");

        if (fgets(filename, MAX_FILENAME_LENGTH, stdin) == NULL) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < MAX_MINOTAUR; ++i) {
        minotaurs[i] = -1;
    }

    sigemptyset(&action.sa_mask);
    action.sa_handler = handler;

    if (sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "An error occurred while catching the SIGINT signal\n");
        exit(EXIT_FAILURE);
    }

    mapFd = loadGame(filename);

    shmId = createSegment(KEY_SEGMENT, sizeof(map_t), S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL, false);

    if (shmId == -2) {
        printf("Shared memory segment already exists, opening\n");

        shmId = openSegment(KEY_SEGMENT);
    }

    map = (map_t*) attachSegment(shmId, 0);

    /* Write the map (loaded from file) into the shared memory segment */
    readFileOff(mapFd, &map->mapVersion, 0, SEEK_SET, sizeof(map->mapVersion));
    readFileOff(mapFd, &map->lives, 0, SEEK_CUR, sizeof(map->lives));
    readFileOff(mapFd, map->map, 0, SEEK_CUR, sizeof(map->map) * MAP_HEIGHT * MAP_WIDTH);

    /* Displays map, temporary */
    for (i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i) {
        printf("%d ", map->map[i]);

        if ((i + 1) % MAP_WIDTH == 0) {
            printf("\n");
        }
    }

    /*printf("Map version: %d\n", map->mapVersion);
    printf("Lives: %d\n", map->lives);*/

    queueId = createMessageQueue(KEY_MESSAGE_QUEUE, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL, false);

    if (queueId == -2) {
        printf("Queue already exists, opening\n");

        queueId = openMessageQueue(KEY_MESSAGE_QUEUE);
    }


    semId = createSemaphores(KEY_SEMAPHORES, 3, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL, false);

    if (semId == -2) {
        printf("Array of semaphores already exists, opening\n");

        semId = openSemaphores(KEY_SEMAPHORES);
    }

    init(semId, values);

    printf("Sem id: %d, Shm id: %d, Queue id: %d\n", semId, shmId, queueId);

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
                    response.keySem = semId;
                    response.keyShm = shmId;

                    sendMessage(queueId, &response, sizeof(response_t), 0);

                    break;

                case CONNECT_TYPE:
                    printf("Received connect request from PID %d\n", request.message.connect.pid);

                    switch (request.message.connect.programType) {
                        case PLAYER:
                            player = request.message.connect.pid;

                            printf("Player %d just connected\n", player);

                            break;

                        case MINOTAUR:
                            /*printf("Minotaur just connected, i = %d\n", i);*/
                            minotaurs[i] = request.message.connect.pid;
                            i++;

                            break;
                    }

                    break;

                case DISCONNECT_TYPE:
                    /*printf("Received disconnect request from PID %d\n", request.message.disconnect.pid);*/

                    switch (request.message.disconnect.programType) {
                        case PLAYER:
                            printf("Player just disconnected\n");

                            player = -1;

                            break;

                        case MINOTAUR:
                            for (j = 0; j < MAX_MINOTAUR && minotaurs[j] != request.message.disconnect.pid; ++j);

                            if (j <= MAX_MINOTAUR) {
                                /* If we found a corresponding pid, replace it by the last inserted pid */
                                i--;
                                minotaurs[j] = minotaurs[i];
                                minotaurs[i] = -1;

                                /*printf("Minotaur just stopped, i = %d\n", i);*/
                            }

                            break;
                    }

                    break;
            }
        }
    }

    /*printf("Received SIGINT from keyboard\n");*/

    for (i = 0; i < MAX_MINOTAUR; ++i) {
        if (minotaurs[i] != -1) {
            kill(minotaurs[i], SIGINT);
            receiveMessage(queueId, &request, sizeof(request_t), DISCONNECT_TYPE, 0);
            /*printf("Received disconnect confirmation from minotaur PID %d\n", request.message.disconnect.pid);*/
        }
    }

    if (player != -1) {
        kill(player, SIGINT);
        printf("Waiting for msg\n");
        receiveMessage(queueId, &request, sizeof(request_t), DISCONNECT_TYPE, 0);
        printf("Received msg\n");
    }

    removeSegment(shmId);
    removeMessageQueue(queueId);
    removeSemaphores(semId);

    return EXIT_SUCCESS;
}
