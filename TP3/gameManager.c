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
    char *originalMapFilename, *mapName;

    if (strstr(filename, "_game.bin")) {
        /* User specified a save file, load it if it exists */
        saveFd = openFile(filename, O_RDWR, S_IRUSR | S_IWUSR);
    }

    else {
        /* User specified a map name, we create a new save file */

        /* We attempt to open the map file */
        mapFd = openFile(filename, O_RDONLY, S_IRUSR);

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
            copyFile(originalMapFilename, saveFilename);

            /* Write the remaining lives as well as the starting position of the player */
            buf[0] = remainingLives;
            buf[1] = X_POS_BEGINNING;
            buf[2] = Y_POS_BEGINNING;

            writeFileOff(saveFd, buf, 0, SEEK_END, sizeof(unsigned char) * 3);
        }

        free(originalMapFilename);
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
        if ((x != X_POS_BEGINNING || y != Y_POS_BEGINNING) && (x != X_POS_END || y != Y_POS_END)) {
            writeFileOff(fd, &nextType, offset, SEEK_SET, sizeof(unsigned char));
            res = nextType;
        }
    }

    return res;
}

void updateDiscoveredWalls(WINDOW *window, int fd) {
    updateStateWindow(window, 1, 3, "Walls: %d/%d", getWallCount(fd, DISCOVERED_WALL), getWallCount(fd, INVISIBLE_WALL) + getWallCount(fd, DISCOVERED_WALL));
}

void updateMoves(WINDOW *window, int fd) {
    updateStateWindow(window, 1, 1, "Moves: %d", getVisitedSquares(fd));
}

void updateLivesLeft(WINDOW *window, int fd) {
    updateStateWindow(window, 1, 2, "Lives: %d", getRemainingLives(fd));
}

/*unsigned char getPlayerXPosition(int fd) {
    unsigned char xPos;
    int initialPadding = sizeof(int) + sizeof(unsigned char), offset;
    offset = initialPadding + (MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char)) + sizeof(unsigned char);

    if (readFileOff(fd, &xPos, offset, SEEK_SET, sizeof(unsigned char)) == 0) {
        xPos = 0;
    }

    return xPos;
}

unsigned char getPlayerYPosition(int fd) {
    unsigned char yPos;
    int initialPadding = sizeof(int) + sizeof(unsigned char), offset;
    offset = initialPadding + (MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char)) + sizeof(unsigned char) + sizeof(unsigned char);

    if (readFileOff(fd, &yPos, offset, SEEK_SET, sizeof(unsigned char)) == 0) {
        yPos = 0;
    }

    return yPos;
}*/
