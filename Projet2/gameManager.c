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
#include "mapUtils.h"
#include "windowDrawer.h"

int loadGame(char *filename) {
    int saveFd, mapFd, mapVersion;
    unsigned char remainingLives;
    unsigned char buf[3];
    char saveFilename[MAX_FILENAME_LENGTH];
    char *originalMapFilename, *mapName, *path;

    if (strstr(filename, "_game.bin")) {
        /* User specified a save file, load it if it exists */
        saveFd = openFile(SAVES_FOLDER, filename, O_RDWR, S_IRUSR | S_IWUSR);
    }

    else {
        /* User specified a map name, we create a new save file */

        /* We attempt to open the map file */
        mapFd = openFile(MAPS_FOLDER, filename, O_RDONLY, S_IRUSR);

        originalMapFilename = (char*) malloc((strlen(filename) + 1) * sizeof(char));

        if(originalMapFilename == NULL) {
            stop_ncurses();
            fprintf(stderr, "An error occurred while trying to allocate memory\n");
            exit(EXIT_FAILURE);
        }

        strcpy(originalMapFilename, filename);

        /* Separate the map name from the extension */
        mapName = strtok(filename, ".");

        mapVersion = getMapVersion(mapFd);
        remainingLives = getTotalLives(mapFd);
        closeFile(mapFd);

        sprintf(saveFilename, "%s_%d_game.bin", mapName, mapVersion);

        path = getPath(SAVES_FOLDER, saveFilename);

        /* Check if save file already exists */
        if ((saveFd = open(path, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1) {
            if (errno != EEXIST) {
                stop_ncurses();
                perror("An error occurred while trying to open the file");
                exit(EXIT_FAILURE);
            }

            saveFd = openFile(SAVES_FOLDER, saveFilename, O_RDWR, S_IRUSR | S_IWUSR);
        }

        else {
            copyFile(MAPS_FOLDER, originalMapFilename, SAVES_FOLDER, saveFilename);

            /* Write the remaining lives as well as the starting position of the player */
            buf[0] = remainingLives;
            buf[1] = X_COORDINATE_ENTRANCE;
            buf[2] = Y_COORDINATE_ENTRANCE;

            writeFileOff(saveFd, buf, 0, SEEK_END, sizeof(unsigned char) * 3);
        }

        free(path);
        free(originalMapFilename);
    }

    return saveFd;
}

unsigned char movePlayer(int fd, int newX, int newY) {
    unsigned char wall, newSquare = UNCHANGED;
    int initialPadding = sizeof(int) + sizeof(unsigned char), offset;
    ssize_t bytesRead;

    if (newX >= 0 && newX < MAP_WIDTH && newY >= 0 && newY < MAP_HEIGHT) {
        offset = initialPadding + (newY * MAP_WIDTH * sizeof(unsigned char) + newX * sizeof(unsigned char));
        bytesRead = readFileOff(fd, &wall, offset, SEEK_SET, sizeof(unsigned char));

        if (bytesRead > 0) {
            switch (wall) {
                case INVISIBLE_WALL:
                    /* Player loses one life, discovers one wall */
                    newSquare = DISCOVERED_WALL;

                    changeWallGame(fd, newX, newY);
                    loseLife(fd);

                    break;

                case VISITED_SQUARE:
                case EMPTY_SQUARE:
                    newSquare = VISITED_SQUARE;

                    changeWallGame(fd, newX, newY);
                    setPlayerPosition(fd, newX, newY);

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

void loadStateWindowGame(WINDOW *window) {
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
    mvwprintw(window, 7, 4, "Player");

    mvwprintw(window, 8, 1, "E ");
    mvwprintw(window, 8, 4, "Exit");

    wrefresh(window);
}

unsigned char getNextWallGame(unsigned char type) {
    unsigned char nextType;

    switch(type) {
        case EMPTY_SQUARE:
            nextType = VISITED_SQUARE;
            break;

        case INVISIBLE_WALL:
            nextType = DISCOVERED_WALL;
            break;

        case VISIBLE_WALL:
        case VISITED_SQUARE:
        default:
            nextType = type;
    }

    return nextType;
}

unsigned char changeWallGame(int fd, int x, int y) {
    unsigned char originalType, nextType, res = UNCHANGED;
    /* Map version + number of lives */
    int initialPadding = sizeof(int) + sizeof(unsigned char), offset;

    /* If the given coordinates are within the map */
    if ((x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)) {
        offset = initialPadding + (y * MAP_WIDTH * sizeof(unsigned char) + x * sizeof(unsigned char));

        readFileOff(fd, &originalType, offset, SEEK_SET, sizeof(unsigned char));

        nextType = getNextWallGame(originalType);

        /* If the coordinates aren't corresponding to the entry or exit we write the type to the corresponding position */
        if ((x != X_COORDINATE_ENTRANCE || y != Y_COORDINATE_ENTRANCE) && (x != X_COORDINATE_EXIT || y != Y_COORDINATE_EXIT)) {
            writeFileOff(fd, &nextType, offset, SEEK_SET, sizeof(unsigned char));
            res = nextType;
        }
    }

    return res;
}


void changeAllWalls(int fd, unsigned char type) {
    unsigned char buffer[MAP_WIDTH * MAP_HEIGHT], nextType = getNextWallGame(type);
    /* Map version + number of lives */
    int initialPadding = sizeof(int) + sizeof(unsigned char), i;
    ssize_t bytesRead;

    if ((bytesRead = readFileOff(fd, buffer, initialPadding, SEEK_SET, MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char))) > 0) {
        for (i = 0; i < bytesRead; ++i) {
            if (buffer[i] == type) {
                buffer[i] = nextType;
            }
        }

        writeFileOff(fd, buffer, initialPadding, SEEK_SET, MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char));
    }

}

void updateDiscoveredWalls(WINDOW *window, int fd) {
    updateStateWindow(window, 1, 3, "Walls: %d/%d", getWallCount(fd, DISCOVERED_WALL), getWallCount(fd, INVISIBLE_WALL) + getWallCount(fd, DISCOVERED_WALL));
}

void updateMoves(WINDOW *window, int fd) {
    updateStateWindow(window, 1, 1, "Moves: %d", getWallCount(fd, VISITED_SQUARE));
}

void updateLivesLeft(WINDOW *window, int fd) {
    updateStateWindow(window, 1, 2, "Lives: %d/%d", getRemainingLives(fd), getTotalLives(fd));
}

void discoverAllWalls(WINDOW *window, int fd) {
    changeAllWalls(fd, INVISIBLE_WALL);
    drawMap(window, fd);
}

void getPlayerPosition(int fd, unsigned char *x, unsigned char *y) {
    int initialPadding = sizeof(int) + sizeof(unsigned char), offset;
    offset = initialPadding + (MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char)) + sizeof(unsigned char);

    if (readFileOff(fd, x, offset, SEEK_SET, sizeof(unsigned char)) == 0) {
        *x = X_COORDINATE_ENTRANCE;
    }

    if (readFile(fd, y, sizeof(unsigned char)) == 0) {
        *y = Y_COORDINATE_ENTRANCE;
    }
}

void setPlayerPosition(int fd, int x, int y) {
    unsigned char buf[2];
    int initialPadding = sizeof(int) + sizeof(unsigned char), offset;
    offset = initialPadding + (MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char)) + sizeof(unsigned char);

    buf[0] = (unsigned char) x;
    buf[1] = (unsigned char) y;

    writeFileOff(fd, buf, offset, SEEK_SET, 2 * sizeof(unsigned char));
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

ssize_t setRemainingLives(int fd, unsigned char lives) {
    ssize_t bytesWritten;
    int initialPadding = sizeof(int) + sizeof(unsigned char), offset;
    offset = initialPadding + (MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char));

    bytesWritten = writeFileOff(fd, &lives, offset, SEEK_SET, sizeof(unsigned char));

    return bytesWritten;
}