#ifndef TP3_GAMEMANAGER_H
#define TP3_GAMEMANAGER_H

#include <curses.h>
#include <sys/types.h>

int loadGame(char *filename);
int movePlayer(int fd, int newX, int newY);
int loseLife(int fd);
unsigned char getRemainingLives(int fd);
ssize_t setRemainingLives(int fd, unsigned char lives);
int getVisitedSquares(int fd);
void loadStateWindowManager(WINDOW *window, int fd);
void updateStateWindowManager(WINDOW *window, int x, int y, char *s, int value);
int changeWallManager(int fd, int x, int y);

#endif
