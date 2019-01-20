#ifndef __MAPEDITOR_H__
#define __MAPEDITOR_H__

void initializeMap(int fd);
int loadMap(char *mapName);
int changeWall(int fd, int x, int y);
int setWall(int fd, unsigned char type, int x, int y);
void loadStateWindow(WINDOW *window, int fd);
void updateStateWindow(WINDOW *window, int x, int y, char *s, int value);
void updateWallCount(WINDOW *window, int fd);
void updateLivesCount(WINDOW *window, int fd);
void increaseLives(int fd);
void decreaseLives(int fd);
int getWallCount(int fd);
unsigned char getLives(int fd);

#endif