#include "structures.h"
#include "messageQueueUtils.h"
#include "semaphoreUtils.h"
#include "sharedMemoryUtils.h"
#include "gameManager.h"
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

bool run = TRUE;

void handler(int sig) {
    if (sig == SIGINT) {
        run = FALSE;
    }
}

int main(int argc, char *argv[]) {
    int keyQueue, queueId, semId, shmId, initialPos, nextPos, pos, i, startPosX = -1, startPosY = -1, x, y, chunk;
    /* int up, down, right, left, leftUp, leftDown, rightUp, rightDown, chunkUp, chunkDown, chunkRight, chunkLeft, chunkLeftUp, chunkLeftDown, chunkRightUp, chunkRightDown, */
    int length;
    int differentChunks[9], *unique;
    unsigned char myId, previousSquare, atPos;
    request_t request;
    response_t response;
    struct sigaction action;
    map_t *map;

    srand((unsigned int) (time(NULL) + getpid()));

    if (argc >= 2) {
        keyQueue = atoi(argv[1]);

        if (argc == 4) {
            startPosX = atoi(argv[2]);
            startPosY = atoi(argv[3]);
        }
    }

    else {
        printf("Please enter the message queue key: ");

        if (scanf("%d", &keyQueue) != 1) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }
    }

    sigemptyset(&action.sa_mask);
    action.sa_handler = handler;

    if (sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "An error occurred while catching the SIGINT signal\n");
        exit(EXIT_FAILURE);
    }

    request.type = GET_KEYS_TYPE;
    queueId = openMessageQueue(keyQueue);
    sendMessage(queueId, &request, sizeof(request_t), 0);

    receiveMessage(queueId, &response, sizeof(response_t), RESPONSE_TYPE, 0);

    semId = response.semId;
    shmId = response.shmId;

    P(semId, SEM_MINOTAUR, 1);

    request.type = CONNECT_TYPE;
    request.message.connect.pid = getpid();
    request.message.connect.programType = MINOTAUR;
    sendMessage(queueId, &request, sizeof(request_t), 0);

    map = (map_t*) attachSegment(shmId, 0);

    myId = (unsigned char) (MINOTAUR_ID_BEGIN + (MAX_MINOTAUR - (getSemaphoreValue(semId, SEM_MINOTAUR) + 1)));

    /* Displays the map in text form */
    for (i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i) {
        x = i % 30;
        y = i / 30;

        chunk = (((y / CHUNK_SIZE) + 1) * ((x / CHUNK_SIZE) + 1)) - 1;

        P(semId, SEM_MAP_CHUNK + chunk, 1);
        printf("%d ", map->map[i]);
        V(semId, SEM_MAP_CHUNK + chunk, 1);

        if ((i + 1) % MAP_WIDTH == 0) {
            printf("\n");
        }
    }

    if (startPosX == -1 && startPosY == -1) {
        do {
            initialPos = rand() % (MAP_WIDTH * MAP_HEIGHT);

            x = initialPos % 30;
            y = initialPos / 30;
            chunk = (((y / CHUNK_SIZE) + 1) * (((x) / CHUNK_SIZE) + 1)) - 1;

            P(semId, SEM_MAP_CHUNK + chunk, 1);
            atPos = map->map[initialPos];
            V(semId, SEM_MAP_CHUNK + chunk, 1);

            printf("Pos: %d, At initialPos: %d\n", initialPos, atPos);
        } while (atPos != EMPTY_SQUARE && atPos != VISITED_SQUARE);
    }

    else {
        initialPos = startPosY * MAP_WIDTH * sizeof(unsigned char) + startPosX * sizeof(unsigned char);

        if (initialPos < MAP_WIDTH * MAP_HEIGHT && initialPos >= 0) {
            chunk = (((startPosY / CHUNK_SIZE) + 1) * (((startPosX) / CHUNK_SIZE) + 1)) - 1;
            P(semId, SEM_MAP_CHUNK + chunk, 1);
            atPos = map->map[initialPos];
            V(semId, SEM_MAP_CHUNK + chunk, 1);

            if (atPos != EMPTY_SQUARE && atPos != VISITED_SQUARE) {
                fprintf(stderr, "You can't place a minotaur on an occupied square\n");
                goto quit;
            }
        }

        else {
            fprintf(stderr, "The specified coordinates are invalid\n");
            goto quit;
        }
    }

    P(semId, SEM_MAP_CHUNK + chunk, 1);
    previousSquare = map->map[initialPos];
    map->map[initialPos] = myId;
    V(semId, SEM_MAP_CHUNK + chunk, 1);

    nextPos = initialPos;

    while (1) {
        if (!run) {
            break;
        }

        sleep(SLEEP_MINOTAUR);

        pos = rand() % 4;

        switch (pos) {
            case UP:
                nextPos -= MAP_WIDTH;
                break;

            case DOWN:
                nextPos += MAP_WIDTH;
                break;

            case RIGHT:
                nextPos += 1;
                break;

            case LEFT:
                nextPos -= 1;
                break;
        }

        if (nextPos >= 0 && nextPos < MAP_HEIGHT * MAP_WIDTH && nextPos % MAP_WIDTH != 0 && nextPos % MAP_WIDTH != (MAP_WIDTH - 1)) {
            for (i = 0; i < 8; ++i) {
                differentChunks[i] = getChunkAt(nextPos, i);
            }

            chunk = ((((nextPos / MAP_WIDTH) / CHUNK_SIZE) + 1) * ((nextPos % MAP_WIDTH / CHUNK_SIZE) + 1)) - 1;
            differentChunks[8] = chunk;

            /* 8 directions + the current chunk */
            length = 9;
            unique = makeUnique(differentChunks, &length);

            /* Locking every unique chunk that the minotaur can reach in the 8 directions */
            for (i = 0; i < length; ++i) {
                printf("Locking chunk %d\n", unique[i]);
                P(semId, SEM_MAP_CHUNK + unique[i], 1);
            }

            if (map->map[nextPos] == EMPTY_SQUARE || map->map[nextPos] == VISITED_SQUARE) {
                map->map[initialPos] = previousSquare;
                previousSquare = map->map[nextPos];
                map->map[nextPos] = myId;

                initialPos = nextPos;
            }

            else {
                nextPos = initialPos;
            }

            for (i = 0; i < length; ++i) {
                printf("Unlocking chunk %d\n", unique[i]);
                V(semId, SEM_MAP_CHUNK + unique[i], 1);
            }

            free(unique);
        }

        else {
            nextPos = initialPos;
        }
    }

    x = nextPos % 30;
    y = nextPos / 30;
    chunk = (((y / CHUNK_SIZE) + 1) * (((x) / CHUNK_SIZE) + 1)) - 1;

    P(semId, SEM_MAP_CHUNK + chunk, 1);
    map->map[nextPos] = previousSquare;
    V(semId, SEM_MAP_CHUNK + chunk, 1);

quit:
    V(semId, SEM_MINOTAUR, 1);

    request.type = DISCONNECT_TYPE;
    request.message.disconnect.pid = getpid();
    request.message.disconnect.programType = MINOTAUR;
    sendMessage(queueId, &request, sizeof(request_t), 0);

    return EXIT_SUCCESS;
}
