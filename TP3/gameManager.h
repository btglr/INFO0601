#ifndef TP3_GAMEMANAGER_H
#define TP3_GAMEMANAGER_H

#include <curses.h>
#include <sys/types.h>

int loadGame(char *filename);
unsigned char movePlayer(int fd, int newX, int newY);
int loseLife(int fd);
void loadStateWindowGame(WINDOW *window);
unsigned char changeWallGame(int fd, int x, int y);
void changeAllWalls(int fd, unsigned char type);
unsigned char getNextWallGame(unsigned char type);
void updateDiscoveredWalls(WINDOW *window, int fd);
void updateMoves(WINDOW *window, int fd);
void updateLivesLeft(WINDOW *window, int fd);
void discoverAllWalls(WINDOW *window, int fd);
void getPlayerPosition(int fd, unsigned char *x, unsigned char *y);
void setPlayerPosition(int fd, int x, int y);
unsigned char getTotalLives(int fd);
unsigned char getRemainingLives(int fd);

#endif
