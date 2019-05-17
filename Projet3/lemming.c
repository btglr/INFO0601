#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <curses.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include "lemming.h"
#include "structures/constants.h"
#include "utils/chunkManager.h"
#include "utils/memoryUtils.h"
#include "utils/threadUtils.h"

extern WINDOW *gameWindow;
extern map_t *map;
extern pthread_mutex_t displayMutex;

WINDOW *gameWindow;
map_t *map;
pthread_mutex_t displayMutex;

void* lemming_thread(void *arg) {
    bool moved;
    bool run = true;
    int nextPosition;
    int previousChunkId, nextChunkId, previousChunkLocalCoordinate, nextChunkLocalCoordinate;
    unsigned int seed = (unsigned int) (time(NULL) + getpid());
    lemming_data_t *lemmingData;
    coord_t *before, *after;
    lemming_t *previousSquareLemmingPtr;
    square_t *nextSquarePtr, *previousSquarePtr;
    struct timespec timeToWait;
    struct timeval now;
    int returnValue;

    lemmingData = (lemming_data_t*) arg;

    before = lemmingData->coords;
    after = (coord_t*) malloc_check(sizeof(coord_t));

    after->x = before->x;
    after->y = before->y;

    sleep(1);

    while(run) {
        moved = false;
        previousChunkId = getChunk(before, map);

        nextPosition = rand_r(&seed) % 4;

        switch (nextPosition) {
            case UP:
                after->y -= (after->y == 0) ? 0 : 1;
                break;

            case DOWN:
                after->y += (after->y == map->height - 1) ? 0 : 1;
                break;

            case RIGHT:
                after->x += (after->x == map->width - 1) ? 0 : 1;
                break;

            case LEFT:
                after->x -= (after->x == 0) ? 0 : 1;
                break;
        }

        nextChunkId = getChunk(after, map);

        previousChunkLocalCoordinate = globalToLocalCoordinate(before, map->chunkSize);
        nextChunkLocalCoordinate = globalToLocalCoordinate(after, map->chunkSize);

        /* Locking the mutex of the chunk the lemming is moving to */
        mutex_lock_check(&map->chunks[nextChunkId].mutex);

        nextSquarePtr = &map->chunks[nextChunkId].squares[nextChunkLocalCoordinate];

        /* If the previous and current chunk are different, lock the previous one too */
        if (nextChunkId != previousChunkId) {
            mutex_lock_check(&map->chunks[previousChunkId].mutex);
        }

        /* If there is no obstacle/lemming already on the square */
        if (nextSquarePtr->type == EMPTY) {
            fprintf(stderr, "DEBUG | Before: (%d, %d) | After: (%d, %d)\n", before->x, before->y, after->x, after->y);
            fprintf(stderr, "DEBUG | Previous position in chunk %d: %d\n", previousChunkId, previousChunkLocalCoordinate);
            fprintf(stderr, "DEBUG | New position in chunk %d: %d\n", nextChunkId, nextChunkLocalCoordinate);

            before->x = after->x;
            before->y = after->y;

            previousSquarePtr = &map->chunks[previousChunkId].squares[previousChunkLocalCoordinate];
            previousSquareLemmingPtr = previousSquarePtr->lemming;

            previousSquarePtr->type = EMPTY;
            nextSquarePtr->type = LEMMING;

            /* Moving the lemming thread pointer in its new square */
            /*map->chunks[nextChunkId].squares[nextChunkLocalCoordinate].lemming = map->chunks[previousChunkId].squares[previousChunkLocalCoordinate].lemming;
            map->chunks[previousChunkId].squares[previousChunkLocalCoordinate].lemming = NULL;*/

            /*map->chunks[nextChunkId].squares[nextChunkLocalCoordinate].lemming = map->chunks[previousChunkId].squares[previousChunkLocalCoordinate].lemming;*/
            nextSquarePtr->lemming = previousSquareLemmingPtr;

            /*fprintf(stderr, "DEBUG | Lemming id at new pos: %d\n", nextSquareLemmingPtr->id);*/
            fprintf(stderr, "DEBUG | Lemming id at new pos: %d\n", nextSquarePtr->lemming->id);

            previousSquarePtr->lemming = NULL;

            moved = true;
        }

        else {
            after->x = before->x;
            after->y = before->y;
        }

        if (nextChunkId != previousChunkId) {
            mutex_unlock_check(&map->chunks[previousChunkId].mutex);
        }

        mutex_unlock_check(&map->chunks[nextChunkId].mutex);

        if (moved) {
            if (previousChunkId != nextChunkId) {
                updateChunk(gameWindow, map, previousChunkId);
            }

            updateChunk(gameWindow, map, nextChunkId);

            gettimeofday(&now, NULL);
            timeToWait.tv_sec = now.tv_sec + 2;

            mutex_lock_check(&nextSquarePtr->lemming->action.mutex);

            returnValue = 0;

            /*fprintf(stderr, "Freeze lemming %d ? %d\n", nextSquarePtr->lemming->id, nextSquarePtr->lemming->action.value);*/

            while (nextSquarePtr->lemming->action.value == 0 && returnValue == 0) {
                if ((returnValue = pthread_cond_timedwait(&nextSquarePtr->lemming->action.cond, &nextSquarePtr->lemming->action.mutex, &timeToWait)) != 0) {
                    if (returnValue != ETIMEDOUT) {
                        fprintf(stderr, "An error occurred while waiting for the condition: %d\n", returnValue);
                        exit(EXIT_FAILURE);
                    }
                }

                else {
                    /* Will exit the loop during the next iteration */
                    run = false;

                    switch (nextSquarePtr->lemming->action.value) {
                        case FREEZE:
                            /* Wait until explosion */
                            while (nextSquarePtr->lemming->action.value != EXPLODE) {
                                pthread_cond_wait(&nextSquarePtr->lemming->action.cond, &nextSquarePtr->lemming->action.mutex);
                            }

                            /* No break, intended */

                        case EXPLODE:
                            fprintf(stderr, "Boom. Explosion.\n");

                            map->chunks[nextChunkId].squares[nextChunkLocalCoordinate].type = EMPTY;

                            /* TODO If killed enemy lemmings, revive it */
                            nextSquarePtr->lemming->dead = TRUE;
                            nextSquarePtr->lemming->action.value = 0;

                            updateChunk(gameWindow, map, nextChunkId);
                            break;
                    }
                }
            }

            mutex_unlock_check(&nextSquarePtr->lemming->action.mutex);
        }
    }

    fprintf(stderr, "The End\n");

    free(after);
    free(lemmingData);

    return NULL;
}