#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "gameManager.h"
#include "fileUtils.h"
#include "constants.h"
#include "ncurses.h"
#include "mapEditor.h"

int loadGame(char *filename) {
    int saveFd, mapFd, mapVersion;
    unsigned char remainingLives;
    unsigned char buf[3];
    char saveFilename[MAX_FILENAME_LENGTH];
    char *originalFilename, *mapName;

    if (strstr(filename, "_game.bin")) {
        /* User specified a save file, load it if it exists */
        saveFd = openFile(filename, O_RDWR, S_IRUSR | S_IWUSR);
    }

    else {
        /* User specified a map name, we create a new save file */

        /* We attempt to open the map file */
        mapFd = openFile(filename, O_RDONLY, S_IRUSR);

        originalFilename = (char*) malloc((strlen(filename) + 1) * sizeof(char));

        if(originalFilename == NULL) {
            stop_ncurses();
            fprintf(stderr, "An error occurred while trying to allocate memory\n");
            exit(EXIT_FAILURE);
        }

        strcpy(originalFilename, filename);

        /* Separate the map name from the extension */
        mapName = strtok(filename, ".");

        readFileOff(mapFd, &mapVersion, 0, SEEK_SET, sizeof(int));
        readFileOff(mapFd, &remainingLives, 0, SEEK_CUR, sizeof(unsigned char));
        closeFile(mapFd);

        sprintf(saveFilename, "%s_%d_game.bin", mapName, mapVersion);

        /* Check if save file already exists */
        if ((saveFd = open(saveFilename, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1) {
            if (errno != EEXIST) {
                stop_ncurses();
                perror("An error occurred while trying to open the file");
                exit(EXIT_FAILURE);
            }

            saveFd = openFile(saveFilename, O_RDWR, S_IRUSR | S_IWUSR);
        }

        else {
            copyFile(originalFilename, saveFilename);

            /* Write the remaining lives as well as the starting position of the player */
            buf[0] = remainingLives;
            buf[1] = X_POS_BEGINNING;
            buf[2] = Y_POS_BEGINNING;

            writeFileOff(saveFd, buf, 0, SEEK_END, sizeof(unsigned char) * 3);
        }

        free(originalFilename);
    }

    return saveFd;
}

int movePlayer(int fd, int newX, int newY) {
    unsigned char wall;
    int initialPadding = sizeof(int) + sizeof(unsigned char), offset, newSquare = -1;
    ssize_t bytesRead;

    if (newX >= 0 && newX < MAP_WIDTH && newY >= 0 && newY < MAP_HEIGHT) {
        offset = initialPadding + (newY * MAP_WIDTH * sizeof(unsigned char) + newX * sizeof(unsigned char));
        bytesRead = readFileOff(fd, &wall, offset, SEEK_SET, sizeof(unsigned char));

        if (bytesRead > 0) {
            switch (wall) {
                case INVISIBLE_WALL:
                    /* Player loses one life, discovers one wall */
                    newSquare = DISCOVERED_WALL;

                    changeWallManager(fd, newX, newY);
                    loseLife(fd);

                    break;

                case VISITED_SQUARE:
                case EMPTY_SQUARE:
                    newSquare = VISITED_SQUARE;

                    changeWallManager(fd, newX, newY);

                    break;

                case DISCOVERED_WALL:
                case VISIBLE_WALL:
                default:
                    break;
            }
        }
    }

    return newSquare;
}

int loseLife(int fd) {
    unsigned char remainingLives = getRemainingLives(fd);

    if (remainingLives > 0) {
        remainingLives--;
        setRemainingLives(fd, remainingLives);
    }

    /* If the return value is 0 then the player has lost */
    return remainingLives;
}

ssize_t setRemainingLives(int fd, unsigned char lives) {
    ssize_t bytesWritten;
    int initialPadding = sizeof(int) + sizeof(unsigned char), offset;
    offset = initialPadding + (MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char));

    bytesWritten = writeFileOff(fd, &lives, offset, SEEK_SET, sizeof(unsigned char));

    return bytesWritten;
}

unsigned char getRemainingLives(int fd) {
    unsigned char remainingLives;
    int initialPadding = sizeof(int) + sizeof(unsigned char), offset;
    offset = initialPadding + (MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char));

    if (readFileOff(fd, &remainingLives, offset, SEEK_SET, sizeof(unsigned char)) == 0) {
        remainingLives = 0;
    }

    return remainingLives;
}

int getVisitedSquares(int fd) {
    ssize_t bytesRead;
    unsigned char buffer[MAP_WIDTH * MAP_HEIGHT];
    int i, visitedSquares = 0;
    int initialPadding = sizeof(int) + sizeof(unsigned char);

    if ((bytesRead = readFileOff(fd, buffer, initialPadding, SEEK_SET, MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char))) > 0) {
        for (i = 0; i < bytesRead; ++i) {
            if (buffer[i] == VISITED_SQUARE) {
                visitedSquares++;
            }
        }
    }

    return visitedSquares;
}

void loadStateWindowManager(WINDOW *window, int fd) {
    updateStateWindowManager(window, 1, 1, "Moves: ", getVisitedSquares(fd));
    updateStateWindowManager(window, 1, 2, "Lives: ", getRemainingLives(fd));
    updateStateWindowManager(window, 1, 3, "Walls: ", getWallCount(fd));

    wattron(window, COLOR_PAIR(PAIR_COLOR_VISIBLE_WALL));
    mvwprintw(window, 5, 1, "  ");
    wattroff(window, COLOR_PAIR(PAIR_COLOR_VISIBLE_WALL));
    mvwprintw(window, 5, 4, "Visible wall");

    wattron(window, COLOR_PAIR(PAIR_COLOR_DISCOVERED_WALL));
    mvwprintw(window, 6, 1, "  ");
    wattroff(window, COLOR_PAIR(PAIR_COLOR_DISCOVERED_WALL));
    mvwprintw(window, 6, 4, "Discovered wall");

    wattron(window, COLOR_PAIR(PAIR_COLOR_PLAYER));
    mvwprintw(window, 7, 1, "  ");
    wattroff(window, COLOR_PAIR(PAIR_COLOR_PLAYER));
    mvwprintw(window, 7, 4, "Discovered wall");

    mvwprintw(window, 8, 1, "E ");
    mvwprintw(window, 8, 4, "Exit");

    wrefresh(window);
}

void updateStateWindowManager(WINDOW *window, int x, int y, char *s, int value) {
    size_t length = strlen(s);

    mvwprintw(window, y, x, s);

    /* Print additional spaces to clear any remaining numbers */
    /* TODO clrtoeol */
    mvwprintw(window, y, (int) (x + length), "%d  ", value);

    wrefresh(window);
}

int changeWallManager(int fd, int x, int y) {
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
                type = VISITED_SQUARE;
                break;

            case INVISIBLE_WALL:
                type = DISCOVERED_WALL;
                break;

            case VISIBLE_WALL:
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