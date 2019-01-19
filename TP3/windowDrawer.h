#ifndef TP3_WINDOWDRAWER_H
#define TP3_WINDOWDRAWER_H

WINDOW* initializeWindow(int width, int height, int x, int y);
WINDOW* initializeSubWindow(WINDOW *window, int width, int height, int x, int y);
void drawMap(WINDOW *window, int fd);

#endif
