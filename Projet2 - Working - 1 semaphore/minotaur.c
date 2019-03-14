#undef _GNU_SOURCE
#define _GNU_SOURCE

#include "structures.h"
#include "messageQueueUtils.h"
#include "semaphoreUtils.h"
#include "sharedMemoryUtils.h"
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

bool run = TRUE;

void handler(int sig) {
    if (sig == SIGINT) {
        run = FALSE;
    }
}

int main(int argc, char *argv[]) {
    int keyQueue, queueId, semId, shmId, initialPos, currentPos, pos, i, startPosX = -1, startPosY = -1;
    unsigned char myId, previousSquare;
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

    P(semId, SEM_MAP, 1);

    /* Displays map, temporary */
    for (i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i) {
        printf("%d ", map->map[i]);

        if ((i + 1) % MAP_WIDTH == 0) {
            printf("\n");
        }
    }

    if (startPosX == -1 && startPosY == -1) {
        do {
            initialPos = rand() % (MAP_WIDTH * MAP_HEIGHT);
            printf("Pos: %d, At initialPos: %d\n", initialPos, map->map[initialPos]);
        } while (map->map[initialPos] != EMPTY_SQUARE && map->map[initialPos] != VISITED_SQUARE);
    }

    else {
        initialPos = startPosY * MAP_WIDTH * sizeof(unsigned char) + startPosX * sizeof(unsigned char);

        if (initialPos > MAP_WIDTH * MAP_HEIGHT || initialPos < 0 || (map->map[initialPos] != EMPTY_SQUARE && map->map[initialPos] != VISITED_SQUARE)) {
            fprintf(stderr, "The specified starting coordinates are invalid\n");

            V(semId, SEM_MAP, 1);

            goto quit;
        }
    }

    previousSquare = map->map[initialPos];
    map->map[initialPos] = myId;

    currentPos = initialPos;

    V(semId, SEM_MAP, 1);

    while (1) {
        if (!run) {
            break;
        }

        /*sleep(SLEEP_MINOTAUR);*/

        pos = rand() % 4;

        switch (pos) {
            case UP:
                currentPos -= MAP_WIDTH;
                break;

            case DOWN:
                currentPos += MAP_WIDTH;
                break;

            case RIGHT:
                currentPos += 1;
                break;

            case LEFT:
                currentPos -= 1;
                break;
        }

        P(semId, SEM_MAP, 1);

        if (currentPos >= 0 && currentPos < MAP_HEIGHT * MAP_WIDTH && currentPos % MAP_WIDTH != 0 && currentPos % MAP_WIDTH != (MAP_WIDTH - 1) && (map->map[currentPos] == EMPTY_SQUARE || map->map[currentPos] == VISITED_SQUARE)) {
            map->map[initialPos] = previousSquare;
            previousSquare = map->map[currentPos];
            map->map[currentPos] = myId;

            initialPos = currentPos;
        }

        else {
            currentPos = initialPos;
        }

        V(semId, SEM_MAP, 1);
    }

    map->map[currentPos] = previousSquare;

quit:
    V(semId, SEM_MINOTAUR, 1);

    request.type = DISCONNECT_TYPE;
    request.message.disconnect.pid = getpid();
    request.message.disconnect.programType = MINOTAUR;
    sendMessage(queueId, &request, sizeof(request_t), 0);

    return EXIT_SUCCESS;
}
