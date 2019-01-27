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

/**
 * Initializes a new map with a default number of lives, a map version and all the squares as 0
 * @param fd A file descriptor to the new map
 */
void initializeMap(int fd) {
    int i, mapVersion;
    unsigned char lives;
    unsigned char buffer[MAP_WIDTH * MAP_HEIGHT];

    for (i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i) {
        buffer[i] = EMPTY_SQUARE;
    }

    /* Set the entrance to a visited square so it is clearly identified */
    buffer[Y_POS_BEGINNING * MAP_WIDTH + X_POS_BEGINNING] = VISITED_SQUARE;

    mapVersion = DEFAULT_MAP_VERSION;
    lives = DEFAULT_LIVES;
    writeFile(fd, &mapVersion, sizeof(int));
    writeFile(fd, &lives, sizeof(unsigned char));
    writeFile(fd, buffer, sizeof(unsigned char) * MAP_WIDTH * MAP_HEIGHT);
}

/**
 * Either loads or creates a new map from given filename
 * @param mapName The map's name
 * @return A file descriptor to the map
 */
int loadMapEditor(char *mapName) {
    int fd;

    fd = open(mapName, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        if(errno == EEXIST) {
            fd = openFile(mapName, O_RDWR);
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
}

void loadStateWindowEditor(WINDOW *window, int fd) {
    updateStateWindow(window, 1, 1, "Lives: %d", getTotalLives(fd));
    updateStateWindow(window, 1, 2, "Walls: %d", getWallCount(fd, -1));

    wattron(window, COLOR_PAIR(PAIR_COLOR_PLUS_SIGN));
    mvwaddch(window, PLUS_SIGN_POS_Y, PLUS_SIGN_POS_X, ACS_HLINE | WA_BOLD);
    mvwaddch(window, PLUS_SIGN_POS_Y, PLUS_SIGN_POS_X + 1, ACS_PLUS | WA_BOLD);
    mvwaddch(window, PLUS_SIGN_POS_Y, PLUS_SIGN_POS_X + 2, ACS_HLINE | WA_BOLD);
    wattroff(window, COLOR_PAIR(PAIR_COLOR_PLUS_SIGN));

    wattron(window, COLOR_PAIR(PAIR_COLOR_MINUS_SIGN));
    mvwaddch(window, MINUS_SIGN_POS_Y, MINUS_SIGN_POS_X, ACS_HLINE | WA_BOLD);
    mvwaddch(window, MINUS_SIGN_POS_Y, MINUS_SIGN_POS_X + 1, ACS_HLINE | WA_BOLD);
    mvwaddch(window, MINUS_SIGN_POS_Y, MINUS_SIGN_POS_X + 2, ACS_HLINE | WA_BOLD);
    wattroff(window, COLOR_PAIR(PAIR_COLOR_MINUS_SIGN));

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

/*int setWall(int fd, unsigned char type, int x, int y) {
    *//* Map version + number of lives *//*
    int initialPadding = sizeof(int) + sizeof(unsigned char);
    int offset;
    ssize_t bytesWritten = -1;

    switch(type) {
        case EMPTY_SQUARE:
        case INVISIBLE_WALL:
        case VISIBLE_WALL:
        case DISCOVERED_WALL:
            *//* If the coordinates aren't corresponding to the entry or exit and are within the map's width and height, we write the type to the corresponding position *//*
            if ((x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) && ((x != X_POS_BEGINNING || y != Y_POS_BEGINNING) && (x != X_POS_END || y != Y_POS_END))) {
                offset = initialPadding + (y * MAP_WIDTH * sizeof(unsigned char) + x * sizeof(unsigned char));
                bytesWritten = writeFileOff(fd, &type, offset, SEEK_SET, sizeof(unsigned char));
            }

            break;

        default:
            printf("Unsupported wall type");
    }

    return (int) bytesWritten;
}*/