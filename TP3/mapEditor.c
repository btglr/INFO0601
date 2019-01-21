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
#include "gameManager.h"
#include "ncurses.h"


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

    mapVersion = 1;
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

int changeWallEditor(int fd, int x, int y) {
    unsigned char originalType, type;
    /* Map version + number of lives */
    int initialPadding = sizeof(int) + sizeof(unsigned char);
    int offset, res;

    /* If the given coordinates are within the map */
    if ((x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)) {
        offset = initialPadding + (y * MAP_WIDTH * sizeof(unsigned char) + x * sizeof(unsigned char));

        readFileOff(fd, &originalType, offset, SEEK_SET, sizeof(unsigned char));

        switch(originalType) {
            case EMPTY_SQUARE:
                type = INVISIBLE_WALL;
                break;

            case INVISIBLE_WALL:
                type = VISIBLE_WALL;
                break;

            case VISIBLE_WALL:
                type = EMPTY_SQUARE;
                break;

            case VISITED_SQUARE:
            default:
                type = originalType;
        }

        /* If the coordinates aren't corresponding to the entry or exit we write the type to the corresponding position */
        if ((x != X_POS_BEGINNING || y != Y_POS_BEGINNING) && (x != X_POS_END || y != Y_POS_END)) {
            writeFileOff(fd, &type, offset, SEEK_SET, sizeof(unsigned char));
            res = type;
        }

        else {
            res = -1;
        }
    }

    else {
        res = -1;
    }

    return res;
}

void updateWallCount(WINDOW *window, int fd) {
    updateStateWindowEditor(window, 1, 2, "Walls: ", getWallCount(fd));
}

void updateLivesCount(WINDOW *window, int fd) {
    updateStateWindowEditor(window, 1, 1, "Lives: ", getLives(fd));
}

void decreaseLives(int fd) {
    unsigned char lives = getLives(fd);

    if(lives > 0) {
        lives--;
        writeFileOff(fd, &lives, sizeof(int), SEEK_SET, sizeof(unsigned char));
    }
}

void increaseLives(int fd) {
    unsigned char lives = getLives(fd);

    if (lives < 255) {
        lives++;
        writeFileOff(fd, &lives, sizeof(int), SEEK_SET, sizeof(unsigned char));
    }
}

int getWallCount(int fd) {
    int i, wallCount = 0;
    int initialPadding = sizeof(int) + sizeof(unsigned char);
    ssize_t bytesRead;
    unsigned char buffer[MAP_WIDTH * MAP_HEIGHT];

    bytesRead = readFileOff(fd, buffer, initialPadding, SEEK_SET, MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char));

    for(i = 0; i < bytesRead; ++i) {
        wallCount = (buffer[i] == VISIBLE_WALL || buffer[i] == INVISIBLE_WALL) ? wallCount + 1 : wallCount;
    }

    return wallCount;
}

unsigned char getLives(int fd) {
    unsigned char lives;

    readFileOff(fd, &lives, sizeof(int), SEEK_SET, sizeof(unsigned char));

    return lives;
}

void loadStateWindowEditor(WINDOW *window, int fd) {
    updateStateWindowEditor(window, 1, 1, "Lives: ", getLives(fd));
    updateStateWindowEditor(window, 1, 2, "Walls: ", getWallCount(fd));

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

void updateStateWindowEditor(WINDOW *window, int x, int y, char *s, int value) {
    size_t length = strlen(s);

    mvwprintw(window, y, x, s);

    /* Print additional spaces to clear any remaining numbers */
    mvwprintw(window, y, (int) (x + length), "%d  ", value);

    wrefresh(window);
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
