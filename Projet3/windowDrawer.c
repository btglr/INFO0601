#include <curses.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "windowDrawer.h"
#include "ncurses.h"
#include "fileUtils.h"
#include "constants.h"

WINDOW* initializeWindow(int width, int height, int x, int y) {
    if ((COLS < x + width) || (LINES < y + height)) {
        stop_ncurses();
        fprintf(stderr, "Terminal window is too small: w=%d, h=%d instead of w=%d, h=%d\n", COLS, LINES, x + width, y + height);
        exit(EXIT_FAILURE);
    }

    return newwin(height, width, y, x);
}

WINDOW *initializeSubWindow(WINDOW *window, int width, int height, int x, int y) {
    return subwin(window, height, width, y, x);
}

void drawSquare(WINDOW *window, int type, int x, int y, bool refresh) {
    chtype color;
    int i;

    switch (type) {
        case OBSTACLE:
            color = COLOR_PAIR(PAIR_COLOR_OBSTACLE);
            break;

        case EMPTY:
        default:
            color = COLOR_PAIR(PAIR_COLOR_EMPTY);
            break;
    }

    wattron(window, color);
    for(i = 0; i < SQUARE_WIDTH; ++i) {
        mvwaddch(window, y, x + i, ' ');
    }
    wattroff(window, color);

    if(refresh) {
        wrefresh(window);
    }
}

void drawMap(WINDOW *window, int mapWidth, int mapHeight, char *map) {
    int i, x, y;

    for(i = 0; i < mapWidth * mapHeight; ++i) {
        x = (i % mapWidth) * SQUARE_WIDTH;
        y = i / mapWidth;

        drawSquare(window, map[i], x, y, false);
    }

    wrefresh(window);}

void printInMiddle(WINDOW *window, int maxWidth, int maxHeight, char *text) {
    int length = (int) strlen(text);

    mvwprintw(window, maxHeight / 2, (maxWidth * SQUARE_WIDTH) / 2 - length / 2, text);
    wrefresh(window);
}

void updateStateWindow(WINDOW *window, int x, int y, char *s, ...) {
    char buffer[256] = {0};
    char tmpBuffer[32] = {0};
    va_list va;

    va_start(va, s);

    while (*s) {
        if (*s == '%') {
            s++;

            if (*s == 'd') {
                sprintf(tmpBuffer, "%d", va_arg(va, int));
                s++;
            }
            else if (*s == 'c') {
                sprintf(tmpBuffer, "%c", va_arg(va, int));
                s++;
            }
            else if (*s == 's') {
                sprintf(tmpBuffer, "%s", va_arg(va, const char*));
                s++;
            }
        }

        else {
            sprintf(tmpBuffer, "%c", *s);
            s++;
        }

        sprintf(buffer, "%s%s", buffer, tmpBuffer);
    }

    clearLine(window, x, y);
    mvwprintw(window, y, x, "%s", buffer);

    va_end(va);

    wrefresh(window);
}

void clearLine(WINDOW *window, int x, int y) {
    int previousX, previousY;

    move(y, x);

    getyx(stdscr, previousY, previousX);
    move(previousY, 0);
    wclrtoeol(window);
    move(previousY, previousX);
}
