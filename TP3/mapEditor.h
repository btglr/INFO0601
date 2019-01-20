#ifndef __MAPEDITOR_H__
#define __MAPEDITOR_H__

void initializeMap(int fd);
int loadMap(char *mapName);
int changeWall(int fd, int x, int y);
int setWall(int fd, unsigned char type, int x, int y);
void loadStateWindow(WINDOW *window, int fd);
void updateStateWindow(WINDOW *window, int x, int y, char *s);

#endif