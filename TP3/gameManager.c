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

/**
 * Loads the filename whether it be a save file or a map file
 * @param filename The file to load
 * @return A file descriptor to the file save
 */
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

/**
 * Moves the player to the given coordinates
 * @param fd The file descriptor of the save file
 * @param newX The new X coordinate of the player
 * @param newY The new Y coordinate of the player
 * @return The new square where the player is now positioned, or UNCHANGED
 */
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

/**
 * Removes one life from the player
 * @param fd The file descriptor of the save file
 * @return The number of lives remaining
 */
int loseLife(int fd) {
    unsigned char remainingLives = getRemainingLives(fd);

    if (remainingLives > 0) {
        remainingLives--;
        setRemainingLives(fd, remainingLives);
    }

    /* If the return value is 0 then the player has lost */
    return remainingLives;
}

/**
 * Loads the initial state window for the game executable
 * @param window The window to load it in
 */
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

/**
 * Gets the next wall type for the game executable
 * @param type The current type of the wall
 * @return The next type of the wall
 */
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

/**
 * Changes the wall type for the game executable
 * @param fd The file descriptor of the save file
 * @param x The x position of the wall
 * @param y The y position of the wall
 * @return The new wall type
 */
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

/**
 * Changes all walls of a given type to the next type
 * @param fd The file descriptor of the save file
 * @param type The type of the walls we want to change to the next one
 */
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

/**
 * Updates the state window with the current amount of discovered walls
 * @param window The window to update
 * @param fd The file descriptor of the save file
 */
void updateDiscoveredWalls(WINDOW *window, int fd) {
    updateStateWindow(window, 1, 3, "Walls: %d/%d", getWallCount(fd, DISCOVERED_WALL), getWallCount(fd, INVISIBLE_WALL) + getWallCount(fd, DISCOVERED_WALL));
}

/**
 * Updates the state window with the current amount of moves
 * @param window The window to update
 * @param fd The file descriptor of the save file
 */
void updateMoves(WINDOW *window, int fd) {
    updateStateWindow(window, 1, 1, "Moves: %d", getVisitedSquares(fd));
}

/**
 * Updates the state window with the current and total amount of lives
 * @param window The window to update
 * @param fd The file descriptor of the save file
 */
void updateLivesLeft(WINDOW *window, int fd) {
    updateStateWindow(window, 1, 2, "Lives: %d/%d", getRemainingLives(fd), getTotalLives(fd));
}

/**
 * Discovers all invisible walls (at the end of the game...)
 * @param window The game window to update
 * @param fd The file descriptor of the save file
 */
void discoverAllWalls(WINDOW *window, int fd) {
    changeAllWalls(fd, INVISIBLE_WALL);
    drawMap(window, fd);
}

/**
 * Gets the player position
 * @param fd The file descriptor of the save file
 * @param x The variable that will contain the x position
 * @param y The variable that will contain the y position
 */
void getPlayerPosition(int fd, unsigned char *x, unsigned char *y) {
    int initialPadding = sizeof(int) + sizeof(unsigned char), offset;
    offset = initialPadding + (MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char)) + sizeof(unsigned char);

    if (readFileOff(fd, x, offset, SEEK_SET, sizeof(unsigned char)) == 0) {
        *x = X_POS_BEGINNING;
    }

    if (readFile(fd, y, sizeof(unsigned char)) == 0) {
        *y = Y_POS_BEGINNING;
    }
}

/**
 * Sets the player position
 * @param fd The file descriptor of the save file
 * @param x The new x position
 * @param y The new y position
 */
void setPlayerPosition(int fd, int x, int y) {
    unsigned char buf[2];
    int initialPadding = sizeof(int) + sizeof(unsigned char), offset;
    offset = initialPadding + (MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char)) + sizeof(unsigned char);

    buf[0] = (unsigned char) x;
    buf[1] = (unsigned char) y;

    writeFileOff(fd, buf, offset, SEEK_SET, 2 * sizeof(unsigned char));
}