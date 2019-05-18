#include "gameState.h"
#include "structures/structures.h"
#include "utils/fileUtils.h"
#include "utils/memoryUtils.h"
#include "utils/chunkManager.h"
#include "utils/threadUtils.h"
#include <stdlib.h>
#include <curses.h>
#include <string.h>

extern WINDOW *gameWindow;
extern map_t *map;
extern pthread_mutex_t displayMutex;

WINDOW *gameWindow;
map_t *map;
pthread_mutex_t displayMutex;

void *send_state(void *arg) {
    state_t *state = (state_t*) arg;
    char *buffer;
    size_t length;
    char *currentAddress;
    coord_t *coords;
    int i;
    int chunkId;

    while (1) {
        /* Request type (unsigned char) + 2 * NUMBER_LEMMINGS * (State type of the lemming (1 unsigned char) + Position (2 unsigned char)) */
        length = sizeof(unsigned char) + 2 * state->numberLemmings * (sizeof(unsigned char) + 2 * sizeof(unsigned char));
        buffer = (char*) malloc_check(length);

        buffer[0] = TYPE_GAME_STATE;

        /* Since we don't know which chunk the lemmings are on we lock them all */
        lockAllChunks(map);

        currentAddress = buffer + sizeof(unsigned char);
        for (i = 0; i < state->numberLemmings; ++i) {
            coords = state->lemmings[i].globalCoordinates;
            /*chunkId = getChunk(coords, map);

            mutex_lock_check(&map->chunks[chunkId].mutex);*/

            memcpy(currentAddress, &state->lemmings[i].state, sizeof(unsigned char));
            memcpy(currentAddress + sizeof(unsigned char), &coords->x, sizeof(unsigned char));
            memcpy(currentAddress + 2 * sizeof(unsigned char), &coords->y, sizeof(unsigned char));

            /*mutex_unlock_check(&map->chunks[chunkId].mutex);*/

            if (i < state->numberLemmings - 1) {
                currentAddress = currentAddress + 2 * sizeof(unsigned char);
            }
        }

        unlockAllChunks(map);

        writeFile(state->socketFd, buffer, length);
        free(buffer);

        sleep(2);
    }

    return NULL;
}
