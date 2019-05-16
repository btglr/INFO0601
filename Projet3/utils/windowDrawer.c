#include <curses.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include "windowDrawer.h"
#include "ncurses.h"
#include "fileUtils.h"
#include "../structures/constants.h"
#include "utils.h"
#include "chunkManager.h"

int cpt = 0;

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

    if (type == LEMMING) {
        wattron(window, COLOR_PAIR(PAIR_COLOR_LEMMING));
        wattron(window, WA_BOLD);
        mvwaddch(window, y, x, LEMMING_CHAR);
        mvwaddch(window, y, x + 1, LEMMING_CHAR);
        wattroff(window, WA_BOLD);
        wattroff(window, COLOR_PAIR(PAIR_COLOR_LEMMING));
    }

    else {
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
    }

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

    wrefresh(window);
}

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

WINDOW *createBorderInformationWindow(int mapWidth) {
    WINDOW *window;

    window = initializeWindow(
            mapWidth * SQUARE_WIDTH + BORDER_STATE_WINDOW_WIDTH + BORDER_WIDTH,
            BORDER_INFORMATION_WINDOW_HEIGHT,
            0,
            0);

    box(window, 0, 0);
    mvwprintw(window, 0, 2, "Information");
    wrefresh(window);

    return window;
}

WINDOW *createInformationWindow(WINDOW *border, int mapWidth) {
    WINDOW *window;

    window = initializeSubWindow(border,
            mapWidth * SQUARE_WIDTH + BORDER_STATE_WINDOW_WIDTH + BORDER_WIDTH - 2,
            BORDER_INFORMATION_WINDOW_HEIGHT - 2,
            1,
            1);

    scrollok(window, TRUE);
    wrefresh(window);

    return window;
}

WINDOW *createBorderStateWindow(int mapWidth) {
    WINDOW *window;

    window = initializeWindow(
            BORDER_STATE_WINDOW_WIDTH,
            BORDER_STATE_WINDOW_HEIGHT,
            mapWidth * SQUARE_WIDTH + BORDER_WIDTH,
            BORDER_INFORMATION_WINDOW_HEIGHT);

    box(window, 0, 0);
    mvwprintw(window, 0, 2, "State");
    wrefresh(window);

    return window;
}

WINDOW *createStateWindow(WINDOW *border, int mapWidth) {
    WINDOW *window;
    int i;

    window = initializeSubWindow(border,
            BORDER_STATE_WINDOW_WIDTH - 2,
            BORDER_STATE_WINDOW_HEIGHT - 2,
            mapWidth * SQUARE_WIDTH + BORDER_WIDTH + 1,
            BORDER_INFORMATION_WINDOW_HEIGHT + 1);

    wattron(window, COLOR_PAIR(PAIR_COLOR_LEGEND));
    mvwprintw(window, COMMAND_TOOLS_POS_Y, COMMAND_TOOLS_POS_X, "COMMANDS");
    wattroff(window, COLOR_PAIR(PAIR_COLOR_LEGEND));

    mvwprintw(window, COMMAND_TOOLS_ADD_POS_Y, COMMAND_TOOLS_POS_X, LEGEND_ADD_LEMMING);

    for (i = 0; i < NUMBER_LEMMINGS; ++i) {
        wattron(window, COLOR_PAIR(PAIR_COLOR_LEMMING));
        wattron(window, WA_BOLD);
        mvwaddch(window, COMMAND_TOOLS_ADD_POS_Y, COMMAND_TOOLS_POS_X + (int) strlen(LEGEND_ADD_LEMMING) + (i * 3) + 1, LEMMING_CHAR);
        mvwaddch(window, COMMAND_TOOLS_ADD_POS_Y, COMMAND_TOOLS_POS_X + (int) strlen(LEGEND_ADD_LEMMING) + (i * 3) + 2, LEMMING_CHAR);
        wattroff(window, WA_BOLD);
        wattroff(window, COLOR_PAIR(PAIR_COLOR_LEMMING));
        mvwaddch(window, COMMAND_TOOLS_ADD_POS_Y, COMMAND_TOOLS_POS_X + (int) strlen(LEGEND_ADD_LEMMING) + (i * 3) + 3, ' ');
    }

    mvwprintw(window, COMMAND_TOOLS_REMOVE_POS_Y, COMMAND_TOOLS_POS_X, "- Remove Lemming");
    mvwprintw(window, COMMAND_TOOLS_EXPLODE_POS_Y, COMMAND_TOOLS_POS_X, "@ Explode Lemmings");
    mvwprintw(window, COMMAND_TOOLS_FREEZE_POS_Y, COMMAND_TOOLS_POS_X, "# Freeze Lemming");
    mvwprintw(window, COMMAND_TOOLS_PAUSE_RESUME_POS_Y, COMMAND_TOOLS_POS_X, "PAUSE");

    wattron(window, COLOR_PAIR(PAIR_COLOR_LEGEND));
    mvwprintw(window, LEGEND_POS_Y, LEGEND_POS_X, "LEGEND");
    wattroff(window, COLOR_PAIR(PAIR_COLOR_LEGEND));

    wattron(window, COLOR_PAIR(PAIR_COLOR_LEMMING));
    wattron(window, WA_BOLD);
    mvwaddch(window, LEGEND_POS_Y + 2, LEGEND_POS_X, LEMMING_CHAR);
    mvwaddch(window, LEGEND_POS_Y + 2, LEGEND_POS_X + 1, LEMMING_CHAR);
    wattroff(window, WA_BOLD);
    wattroff(window, COLOR_PAIR(PAIR_COLOR_LEMMING));
    mvwprintw(window, LEGEND_POS_Y + 2, LEGEND_POS_X + 3, "Lemming");

    wattron(window, COLOR_PAIR(PAIR_COLOR_OBSTACLE));
    mvwprintw(window, LEGEND_POS_Y + 3, LEGEND_POS_X, "  ");
    wattroff(window, COLOR_PAIR(PAIR_COLOR_OBSTACLE));
    mvwprintw(window, LEGEND_POS_Y + 3, LEGEND_POS_X + 3, "Obstacle");

    wrefresh(window);

    return window;
}

WINDOW *createBorderGameWindow(int mapWidth, int mapHeight) {
    WINDOW *window;

    window = initializeWindow(
            mapWidth * SQUARE_WIDTH + BORDER_WIDTH,
            mapHeight + BORDER_HEIGHT,
            0,
            BORDER_INFORMATION_WINDOW_HEIGHT);

    box(window, 0, 0);
    mvwprintw(window, 0, 2, "Map Editor");
    wrefresh(window);

    return window;
}

WINDOW *createGameWindow(WINDOW *border, int mapWidth, int mapHeight) {
    WINDOW *window;

    window = initializeSubWindow(border,
            mapWidth * SQUARE_WIDTH + BORDER_WIDTH - 2,
            mapHeight + BORDER_HEIGHT - 2,
            1,
            BORDER_INFORMATION_WINDOW_HEIGHT + 1);

    wrefresh(window);

    return window;
}

void printInformation(WINDOW *window, char *s, ...) {
    char buffer[1024] = {0};
    char tmpBuffer[256] = {0};
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

    va_end(va);

    if (cpt != 0) {
        wprintw(window, "\n");
    }

    wprintw(window, "%s", buffer);
    wrefresh(window);

    cpt++;
}

