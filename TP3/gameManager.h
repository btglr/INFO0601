#ifndef TP3_GAMEMANAGER_H
#define TP3_GAMEMANAGER_H

#include <curses.h>
#include <sys/types.h>

int loadGame(char *filename);
int movePlayer(int fd, int newX, int newY);
int loseLife(int fd);
void loadStateWindowManager(WINDOW *window);
unsigned char changeWallGame(int fd, int x, int y);
unsigned char getNextWallGame(unsigned char type);
void updateDiscoveredWalls(WINDOW *window, int fd);
void updateMoves(WINDOW *window, int fd);
void updateLivesLeft(WINDOW *window, int fd);

#endif
