#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <curses.h>
#include <string.h>
#include "mapEditor.h"
#include "fileUtils.h"
#include "constants.h"
#include "ncurses.h"
#include "mapUtils.h"
#include "windowDrawer.h"

void initializeMap(int fd) {
    int i, mapVersion;
    unsigned char lives;
    unsigned char buffer[MAP_WIDTH * MAP_HEIGHT];

    for (i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i) {
        buffer[i] = EMPTY_SQUARE;
    }

    /* Set the entrance to a visited square so it is clearly identified */
    buffer[Y_COORDINATE_ENTRANCE * MAP_WIDTH + X_COORDINATE_ENTRANCE] = VISITED_SQUARE;

    mapVersion = DEFAULT_MAP_VERSION;
    lives = DEFAULT_LIVES;
    writeFile(fd, &mapVersion, sizeof(int));
    writeFile(fd, &lives, sizeof(unsigned char));
    writeFile(fd, buffer, sizeof(unsigned char) * MAP_WIDTH * MAP_HEIGHT);
}

int loadMapEditor(char *mapName) {
    int fd;
    char* path = getPath(MAPS_FOLDER, mapName);

    fd = open(path, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        if(errno == EEXIST) {
            fd = openFile(MAPS_FOLDER, mapName, O_RDWR);
        }

        else {
            stop_ncurses();
            perror("An error occurred while trying to open a file");
            exit(EXIT_FAILURE);
        }
    }

    else {
        /* Create empty map */
        initializeMap(fd);
    }

    free(path);

    return fd;
}


unsigned char changeWallEditor(int fd, int x, int y) {
    return setWallAt(fd, x, y, getNextWallAt(fd, x, y, TRUE));
}

void updateWallCount(WINDOW *window, int fd) {
    updateStateWindow(window, 1, 2, "Walls: %d", getWallCount(fd, -1));
}

void updateLivesCount(WINDOW *window, int fd) {
    updateStateWindow(window, 1, 1, "Lives: %d", getTotalLives(fd));
    drawPlusSign(window, X_COORDINATE_PLUS_SIGN, Y_COORDINATE_PLUS_SIGN);
    drawMinusSign(window, X_COORDINATE_MINUS_SIGN, Y_COORDINATE_MINUS_SIGN);
}

void loadStateWindowEditor(WINDOW *window, int fd) {
    updateStateWindow(window, 1, 1, "Lives: %d", getTotalLives(fd));
    updateStateWindow(window, 1, 2, "Walls: %d", getWallCount(fd, -1));

    drawPlusSign(window, X_COORDINATE_PLUS_SIGN, Y_COORDINATE_PLUS_SIGN);
    drawMinusSign(window, X_COORDINATE_MINUS_SIGN, Y_COORDINATE_MINUS_SIGN);

    wattron(window, COLOR_PAIR(PAIR_COLOR_VISIBLE_WALL));
    mvwprintw(window, 4, 1, "  ");
    wattroff(window, COLOR_PAIR(PAIR_COLOR_VISIBLE_WALL));
    mvwprintw(window, 4, 4, "Visible wall");

    wattron(window, COLOR_PAIR(PAIR_COLOR_INVISIBLE_WALL));
    mvwprintw(window, 5, 1, "  ");
    wattroff(window, COLOR_PAIR(PAIR_COLOR_INVISIBLE_WALL));
    mvwprintw(window, 5, 4, "Invisible wall");

    mvwprintw(window, 6, 1, "E ");
    mvwprintw(window, 6, 4, "Exit");

    wrefresh(window);
}

unsigned char getNextWallEditor(unsigned char type) {
    unsigned char nextType;

    switch(type) {
        case EMPTY_SQUARE:
            nextType = INVISIBLE_WALL;
            break;

        case INVISIBLE_WALL:
            nextType = VISIBLE_WALL;
            break;

        case VISIBLE_WALL:
            nextType = EMPTY_SQUARE;
            break;

        case VISITED_SQUARE:
        default:
            nextType = type;
    }

    return nextType;
}

void drawPlusSign(WINDOW *window, int x, int y) {
    wattron(window, COLOR_PAIR(PAIR_COLOR_PLUS_SIGN));
    mvwaddch(window, y, x, ACS_HLINE | WA_BOLD);
    mvwaddch(window, y, x + 1, ACS_PLUS | WA_BOLD);
    mvwaddch(window, y, x + 2, ACS_HLINE | WA_BOLD);
    wattroff(window, COLOR_PAIR(PAIR_COLOR_PLUS_SIGN));
    wrefresh(window);
}

void drawMinusSign(WINDOW *window, int x, int y) {
    wattron(window, COLOR_PAIR(PAIR_COLOR_MINUS_SIGN));
    mvwaddch(window, y, x, ACS_HLINE | WA_BOLD);
    mvwaddch(window, y, x + 1, ACS_HLINE | WA_BOLD);
    mvwaddch(window, y, x + 2, ACS_HLINE | WA_BOLD);
    wattroff(window, COLOR_PAIR(PAIR_COLOR_MINUS_SIGN));
}
