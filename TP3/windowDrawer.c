#include <curses.h>
#include <stdlib.h>
#include "windowDrawer.h"
#include "mapEditor.h"
#include "ncurses.h"
#include "fileUtils.h"

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

void drawMap(WINDOW *window, int fd) {
    chtype color;
    int i, x, y, initialPadding = sizeof(int) + sizeof(unsigned char);
    unsigned char buffer[MAP_WIDTH * MAP_HEIGHT];
    ssize_t bytesRead;

    bytesRead = readFileOff(fd, buffer, initialPadding, sizeof(unsigned char) * MAP_WIDTH * MAP_HEIGHT);

    for(i = 0; i < bytesRead; ++i) {
        x = (i % MAP_WIDTH) * SQUARE_WIDTH;
        y = i / MAP_WIDTH;

        switch (buffer[i]) {
            case VISIBLE_WALL:
                color = COLOR_PAIR(5);
                break;

            case DISCOVERED_WALL:
                color = COLOR_PAIR(4);
                break;

            default:
                color = COLOR_PAIR(1);
        }

        wattron(window, color);
        mvwprintw(window, y, x, "  ");
        wattroff(window, color);

        wrefresh(window);
    }
}
