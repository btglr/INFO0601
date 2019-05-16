#ifndef PROJET3_GAMEUTILS_H
#define PROJET3_GAMEUTILS_H

#include "../structures/structures.h"

void initializeGame(int mapWidth, int mapHeight, char *map, WINDOW **borderInformationWindow,
                    WINDOW **informationWindow, WINDOW **borderGameWindow, WINDOW **gameWindow,
                    WINDOW **borderStateWindow, WINDOW **stateWindow);
void placeLemming(WINDOW *window, map_t *map, int posX, int posY);
int getTool(int mouseX, int mouseY);
int makeMultipleOf(int v, int m);

#endif
