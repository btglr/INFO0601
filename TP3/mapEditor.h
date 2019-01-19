#ifndef __MAPEDITOR_H__
#define __MAPEDITOR_H__

#define X_POS_BEGINNING 0
#define Y_POS_BEGINNING 8
#define X_POS_END 29
#define Y_POS_END 8
#define DEFAULT_LIVES 10
#define MAP_WIDTH 30
#define MAP_HEIGHT 15
#define EMPTY_SQUARE 0
#define INVISIBLE_WALL 1
#define VISIBLE_WALL 2
#define DISCOVERED_WALL 3
#define SQUARE_WIDTH 2 /* How big should a square's width be, in characters */
#define COLOR_EMPTY_SQUARE COLOR_WHITE
#define COLOR_VISIBLE_WALL COLOR_BLUE
#define COLOR_DISCOVERED_WALL COLOR_RED
#define COLOR_PLAYER COLOR_GREEN

void initializeMap(int fd);
int loadMap(char *mapName);
int setWall(int fd, unsigned char type, int x, int y);

#endif