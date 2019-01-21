#ifndef TP3_WINDOWDRAWER_H
#define TP3_WINDOWDRAWER_H

WINDOW* initializeWindow(int width, int height, int x, int y);
WINDOW* initializeSubWindow(WINDOW *window, int width, int height, int x, int y);
void drawWall(WINDOW *window, int type, int x, int y, bool refresh);
void drawMap(WINDOW *window, int fd);
void printInMiddle(WINDOW *window, int maxWidth, int maxHeight, char *text);

#endif
