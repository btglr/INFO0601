#include <curses.h>
#include <stdlib.h>
#include <fcntl.h>
#include "windowDrawer.h"
#include "mapEditor.h"
#include "ncurses.h"
#include "fileUtils.h"
#include "constants.h"

WINDOW* initializeWindow(int width, int height, int x, int y) {
    if ((COLS < x + width) || (LINES < y + height)) {
        ncurses_stopper();
        fprintf(stderr, "Terminal window is too small: w=%d, h=%d instead of w=%d, h=%d\n", COLS, LINES, x + width, y + height);
        exit(EXIT_FAILURE);
    }

    return newwin(height, width, y, x);
}

WINDOW *initializeSubWindow(WINDOW *window, int width, int height, int x, int y) {
    /* TODO Bug when drawing a new map (file just created) */

    return subwin(window, height, width, y, x);
}

void drawWall(WINDOW *window, int type, int x, int y, bool refresh) {
    chtype color;

    switch(type) {
        case VISIBLE_WALL:
            color = COLOR_PAIR(PAIR_COLOR_VISIBLE_WALL);
            break;

        case INVISIBLE_WALL:
            color = COLOR_PAIR(PAIR_COLOR_INVISIBLE_WALL);
            break;

        case DISCOVERED_WALL:
            color = COLOR_PAIR(PAIR_COLOR_DISCOVERED_WALL);
            break;

        default:
            color = COLOR_PAIR(1);
    }

    wattron(window, color);
    mvwprintw(window, y, x, "  ");
    wattroff(window, color);

    if(refresh) {
        wrefresh(window);
    }
}

void drawMap(WINDOW *window, int fd) {
    int i, x, y, initialPadding = sizeof(int) + sizeof(unsigned char);
    unsigned char buffer[MAP_WIDTH * MAP_HEIGHT];
    ssize_t bytesRead;

    bytesRead = readFileOff(fd, buffer, initialPadding, sizeof(unsigned char) * MAP_WIDTH * MAP_HEIGHT);

    for(i = 0; i < bytesRead; ++i) {
        x = (i % MAP_WIDTH) * SQUARE_WIDTH;
        y = i / MAP_WIDTH;

        drawWall(window, buffer[i], x, y, false);
    }

    wrefresh(window);
}
