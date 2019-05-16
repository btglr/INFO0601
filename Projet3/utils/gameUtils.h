#ifndef PROJET3_GAMEUTILS_H
#define PROJET3_GAMEUTILS_H

void initializeGame(int mapWidth, int mapHeight, char *map, WINDOW **borderInformationWindow,
                    WINDOW **informationWindow, WINDOW **borderGameWindow, WINDOW **gameWindow,
                    WINDOW **borderStateWindow, WINDOW **stateWindow);
void placeLemming(WINDOW *window, int lemmingNumber, int posX, int posY);
int getTool(int mouseX, int mouseY);
int makeMultipleOf(int v, int m);

#endif
