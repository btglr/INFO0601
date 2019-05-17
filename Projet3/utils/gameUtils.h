#ifndef PROJET3_GAMEUTILS_H
#define PROJET3_GAMEUTILS_H

#include <thread_db.h>
#include "../structures/structures.h"

void initializeGame(int mapWidth, int mapHeight, char *map, WINDOW **borderInformationWindow,
                    WINDOW **informationWindow, WINDOW **borderGameWindow, WINDOW **gameWindow,
                    WINDOW **borderStateWindow, WINDOW **stateWindow);
bool placeLemming(WINDOW *window, lemming_t *lemming, map_t *map, int posX, int posY);
int getTool(int mouseX, int mouseY);
int makeMultipleOf(int v, int m);
void removeLemming(WINDOW *window, lemming_t *lemming, map_t *map, int chunkId, int chunkPos);
void freezeLemming(lemming_t *lemming, map_t *map);
void explodeLemming(WINDOW *window, lemming_t *lemming, map_t *map, int chunkId, int chunkPos);
lemming_t *getLemmingAtPos(lemming_t *lemmings, int pos);
void initializeLemmings(lemming_t *lemmings, int nbLemmings);

#endif
