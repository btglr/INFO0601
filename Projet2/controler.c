#define _XOPEN_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include "ncurses.h"
#include "constants.h"
#include "messageQueueUtils.h"
#include "semaphoreUtils.h"
#include "structures.h"
#include "mapEditor.h"
#include "fileUtils.h"
#include "sharedMemoryUtils.h"
#include "gameManager.h"
#include <sys/ipc.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    int i, queueId, semId, shmId, mapFd;
    char filename[MAX_FILENAME_LENGTH];
    unsigned short values[5] = {2, 0, 1, 1, 0};
    map_t *map;

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

    for (i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i) {
        printf("%d ", map->map[i]);

        if ((i + 1) % MAP_WIDTH == 0) {
            printf("\n");
        }
    }

    printf("Map version: %d\n", map->mapVersion);
    printf("Lives: %d\n", map->lives);

    queueId = createMessageQueue(KEY_MESSAGE_QUEUE, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL, false);

    if (queueId == -2) {
        printf("Queue already exists, opening\n");

        queueId = openMessageQueue(KEY_MESSAGE_QUEUE);
    }

    printf("Queue id: %d\n", queueId);

    semId = createSemaphores(KEY_SEMAPHORES, 5, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL, false);

    if (semId == -2) {
        printf("Array of semaphores already exists, opening\n");

        semId = openSemaphores(KEY_SEMAPHORES);
    }

    init(semId, values);

    removeSegment(shmId);
    removeMessageQueue(queueId);
    removeSemaphores(semId);

    return EXIT_SUCCESS;
}
