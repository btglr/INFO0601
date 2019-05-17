#include <string.h>
#include <curses.h>
#include <pthread.h>
#include "gameUtils.h"
#include "windowDrawer.h"
#include <stdlib.h>
#include <thread_db.h>
#include "../lemming.h"
#include "memoryUtils.h"
#include "chunkManager.h"
#include "threadUtils.h"

extern pthread_mutex_t displayMutex;

pthread_mutex_t displayMutex;

void initializeGame(int mapWidth, int mapHeight, char *map, WINDOW **borderInformationWindow,
                    WINDOW **informationWindow, WINDOW **borderGameWindow, WINDOW **gameWindow,
                    WINDOW **borderStateWindow, WINDOW **stateWindow) {
    *borderInformationWindow = createBorderInformationWindow(mapWidth);
    *informationWindow = createInformationWindow(*borderInformationWindow, mapWidth);
    *borderStateWindow = createBorderStateWindow(mapWidth);
    *stateWindow = createStateWindow(*borderStateWindow, mapWidth);
    *borderGameWindow = createBorderGameWindow(mapWidth, mapHeight);
    *gameWindow = createGameWindow(*borderGameWindow, mapWidth, mapHeight);

    drawMap(*gameWindow, mapWidth, mapHeight, map);
}

int getTool(int mouseX, int mouseY) {
    int tool = 0;

    if (mouseX >= COMMAND_TOOLS_POS_X) {
        switch (mouseY) {
            case COMMAND_TOOLS_ADD_POS_Y:
                tool = TOOL_ADD;
                break;

            case COMMAND_TOOLS_REMOVE_POS_Y:
                tool = TOOL_REMOVE;
                break;

            case COMMAND_TOOLS_EXPLODE_POS_Y:
                tool = TOOL_EXPLODE;
                break;

            case COMMAND_TOOLS_FREEZE_POS_Y:
                tool = TOOL_FREEZE;
                break;

            case COMMAND_TOOLS_PAUSE_RESUME_POS_Y:
                tool = TOOL_PAUSE_RESUME;
                break;

            default:
                tool = 0;
        }
    }

    return tool;
}

int makeMultipleOf(int v, int m) {
    return v - (v % m);
}

bool placeLemming(WINDOW *window, lemming_t *lemming, map_t *map, int posX, int posY) {
    int chunkId, chunkPos;
    lemming_data_t *lemmingData;
    bool placed = false;

    lemmingData = (lemming_data_t*) malloc(sizeof(lemming_data_t));

    lemmingData->coords = (coord_t*) malloc_check(sizeof(coord_t));
    lemmingData->coords->x = posX / SQUARE_WIDTH;
    lemmingData->coords->y = posY;

    lemmingData->pipe = lemming->pipe;

    chunkId = getChunk(lemmingData->coords, map);

    chunkPos = globalToLocalCoordinate(lemmingData->coords, map->chunkSize);

    if (!lemming->dead && lemming->thread == NULL) {
        map->chunks[chunkId].squares[chunkPos].type = LEMMING;

        updateChunk(window, map, chunkId);

        lemming->thread = (pthread_t*) malloc_check(sizeof(pthread_t));
        create_thread_check(lemming->thread, lemming_thread, lemmingData);

        map->chunks[chunkId].squares[chunkPos].lemming = lemming;
        placed = true;
    }

    return placed;
}

void removeLemming(WINDOW *window, lemming_t *lemming, map_t *map, int chunkId, int chunkPos) {
    pthread_cancel(*lemming->thread);

    map->chunks[chunkId].squares[chunkPos].type = EMPTY;
    map->chunks[chunkId].squares[chunkPos].lemming->timesRemoved += 1;

    updateChunk(window, map, chunkId);

    /*char buf[3];

    write(lemming->pipe[WRITE], "STOP", 5);
    read(lemming->pipe[READ], buf, 3);

    if (strcmp(buf, "OK") == 0) {
        lemming->thread = NULL;
    }*/
}

void initializeLemmings(lemming_t *lemmings, int nbLemmings) {
    int i;

    for (i = 0; i < nbLemmings; ++i) {
        lemmings[i].timesRemoved = 0;
        lemmings[i].id = i;
        lemmings[i].thread = NULL;
        lemmings[i].dead = FALSE;

        pthread_mutex_init(&lemmings[i].action.mutex, NULL);
        pthread_cond_init(&lemmings[i].action.cond, NULL);
        lemmings[i].action.value = false;

        /* TODO Initialize pipe */
    }
}

void freezeLemming(lemming_t *lemming, map_t *map) {
    mutex_lock_check(&lemming->action.mutex);

    lemming->action.value = FREEZE;

    pthread_cond_signal(&lemming->action.cond);

    mutex_unlock_check(&lemming->action.mutex);
}

void explodeLemming(WINDOW *window, lemming_t *lemming, map_t *map, int chunkId, int chunkPos) {
    mutex_lock_check(&lemming->action.mutex);

    lemming->action.value = EXPLODE;

    pthread_cond_signal(&lemming->action.cond);

    mutex_unlock_check(&lemming->action.mutex);

    map->chunks[chunkId].squares[chunkPos].type = EMPTY;

    updateChunk(window, map, chunkId);
}
