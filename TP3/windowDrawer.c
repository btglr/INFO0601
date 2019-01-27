#include <curses.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "windowDrawer.h"
#include "mapEditor.h"
#include "ncurses.h"
#include "fileUtils.h"
#include "constants.h"
#include "mapUtils.h"

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

    switch(type) {
        case INVISIBLE_WALL:
            color = COLOR_PAIR(PAIR_COLOR_INVISIBLE_WALL);
            break;

        case VISIBLE_WALL:
            color = COLOR_PAIR(PAIR_COLOR_VISIBLE_WALL);
            break;

        case DISCOVERED_WALL:
            color = COLOR_PAIR(PAIR_COLOR_DISCOVERED_WALL);
            break;

        case VISITED_SQUARE:
            color = COLOR_PAIR(PAIR_COLOR_VISITED_SQUARE);
            break;

        case PLAYER_SQUARE:
            color = COLOR_PAIR(PAIR_COLOR_PLAYER);
            break;

        case EMPTY_SQUARE:
        default:
            color = COLOR_PAIR(PAIR_COLOR_EMPTY_SQUARE);
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

void drawMap(WINDOW *window, int fd) {
    int i, x, y, initialPadding = sizeof(int) + sizeof(unsigned char);
    unsigned char buffer[MAP_WIDTH * MAP_HEIGHT];
    ssize_t bytesRead;

    bytesRead = readFileOff(fd, buffer, initialPadding, SEEK_SET, sizeof(unsigned char) * MAP_WIDTH * MAP_HEIGHT);

    for(i = 0; i < bytesRead; ++i) {
        x = (i % MAP_WIDTH) * SQUARE_WIDTH;
        y = i / MAP_WIDTH;

        drawSquare(window, buffer[i], x, y, false);
    }

    mvwaddch(window, Y_POS_END, X_POS_END * SQUARE_WIDTH, 'E');

    wrefresh(window);
}

void printInMiddle(WINDOW *window, int maxWidth, int maxHeight, char *text) {
    size_t length = strlen(text);

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
