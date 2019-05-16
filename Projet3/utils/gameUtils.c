#include <curses.h>
#include <pthread.h>
#include "gameUtils.h"
#include "windowDrawer.h"
#include <stdlib.h>
#include "../lemming.h"
#include "memoryUtils.h"
#include "chunkManager.h"

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

void placeLemming(WINDOW *window, map_t *map, int posX, int posY) {
    int chunkId, chunkPos;
    coord_t *coords;

    coords = (coord_t*) malloc_check(sizeof(coord_t));
    coords->x = posX / SQUARE_WIDTH;
    coords->y = posY;

    chunkId = getChunk(coords, map);

    chunkPos = globalToLocalCoordinate(coords, map->chunkSize);

    map->chunks[chunkId].squares[chunkPos].type = LEMMING;
    updateChunk(window, map, chunkId);

    map->chunks[chunkId].squares[chunkPos].lemming = (pthread_t*) malloc_check(sizeof(pthread_t));
    pthread_create(map->chunks[chunkId].squares[chunkPos].lemming, NULL, lemming_thread, coords);
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