#ifndef __MAPEDITOR_H__
#define __MAPEDITOR_H__

#include <curses.h>

void initializeMap(int fd);
int loadMapEditor(char *mapName);
unsigned char changeWallEditor(int fd, int x, int y);
/*int setWall(int fd, unsigned char type, int x, int y);*/
void loadStateWindowEditor(WINDOW *window, int fd);
void updateWallCount(WINDOW *window, int fd);
void updateLivesCount(WINDOW *window, int fd);
unsigned char getNextWallEditor(unsigned char type);

#endif