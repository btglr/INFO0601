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
#include "structures.h"

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

unsigned char movePlayer(map_t *map, unsigned char newX, unsigned char newY) {
    unsigned char wall, newSquare = UNCHANGED;
    int offset;

    if (newX >= 0 && newX < MAP_WIDTH && newY >= 0 && newY < MAP_HEIGHT) {
        offset = newY * MAP_WIDTH * sizeof(unsigned char) + newX * sizeof(unsigned char);
        wall = map->map[offset];

        switch (wall) {
            case INVISIBLE_WALL:
                newSquare = changeWallGame(map, newX, newY);
                /* TODO temporary, remove this */
                /*map->livesLeft--;*/

                break;

            case VISITED_SQUARE:
            case EMPTY_SQUARE:
                newSquare = PLAYER_SQUARE;
                setPlayerPosition(map, newX, newY);

                break;

            case DISCOVERED_WALL:
            case VISIBLE_WALL:
            default:
                break;
        }
    }

    return newSquare;
}

int loseLife(map_t *map) {
    if (map->livesLeft > 0) {
        map->livesLeft--;
    }

    /* If the return value is 0 then the player has lost */
    return map->livesLeft;
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

    wattron(window, COLOR_PAIR(PAIR_COLOR_VISITED_SQUARE));
    mvwprintw(window, 8, 1, "  ");
    wattroff(window, COLOR_PAIR(PAIR_COLOR_VISITED_SQUARE));
    mvwprintw(window, 8, 4, "Trail");

    wattron(window, COLOR_PAIR(PAIR_COLOR_MINOTAUR));
    wattron(window, WA_BOLD);
    mvwaddch(window, 9, 1, MINOTAUR_CHAR);
    mvwaddch(window, 9, 2, MINOTAUR_CHAR);
    wattroff(window, WA_BOLD);
    wattroff(window, COLOR_PAIR(PAIR_COLOR_MINOTAUR));
    mvwprintw(window, 9, 4, "Minotaurs");

    mvwprintw(window, 10, 1, "E ");
    mvwprintw(window, 10, 4, "Exit");

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

unsigned char changeWallGame(map_t *map, int x, int y) {
    unsigned char originalType, nextType, res = UNCHANGED;
    int offset;

    /* If the given coordinates are within the map */
    if ((x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)) {
        offset = y * MAP_WIDTH * sizeof(unsigned char) + x * sizeof(unsigned char);
        originalType = map->map[offset];

        nextType = getNextWallGame(originalType);

        /* If the coordinates aren't corresponding to the entry or exit we write the type to the corresponding position */
        if ((x != X_COORDINATE_ENTRANCE || y != Y_COORDINATE_ENTRANCE) && (x != X_COORDINATE_EXIT || y != Y_COORDINATE_EXIT)) {
            map->map[offset] = nextType;
            res = nextType;
        }
    }

    return res;
}


void changeAllWalls(map_t *map, unsigned char type) {
    unsigned char nextType = getNextWallGame(type);
    int i;

    for (i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i) {
        map->map[i] = (map->map[i] == type) ? nextType : map->map[i];
    }
}

void updateMoves(WINDOW *window, map_t *map) {
    updateStateWindow(window, 1, 1, "Moves: %d", getWallCount(map, VISITED_SQUARE));
}

void updateLivesLeft(WINDOW *window, map_t *map) {
    updateStateWindow(window, 1, 2, "Lives: %d/%d", map->livesLeft, map->lives);
}

void updateDiscoveredWalls(WINDOW *window, map_t *map) {
    updateStateWindow(window, 1, 3, "Walls: %d/%d", getWallCount(map, DISCOVERED_WALL), getWallCount(map, INVISIBLE_WALL) + getWallCount(map, DISCOVERED_WALL));
}

void discoverAllWalls(WINDOW *window, map_t *map) {
    changeAllWalls(map, INVISIBLE_WALL);
    drawMap(window, map);
}

void getPlayerPosition(map_t *map, unsigned char *x, unsigned char *y) {
    /*int initialPadding = sizeof(int) + sizeof(unsigned char), offset;
    offset = initialPadding + (MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char)) + sizeof(unsigned char);

    if (readFileOff(map, x, offset, SEEK_SET, sizeof(unsigned char)) == 0) {
        *x = X_COORDINATE_ENTRANCE;
    }

    if (readFile(map, y, sizeof(unsigned char)) == 0) {
        *y = Y_COORDINATE_ENTRANCE;
    }*/
}

void setPlayerPosition(map_t *map, unsigned char x, unsigned char y) {
    int oldOffset, newOffset;
    oldOffset = map->posY * MAP_WIDTH * sizeof(unsigned char) + map->posX * sizeof(unsigned char);
    newOffset = y * MAP_WIDTH * sizeof(unsigned char) + x * sizeof(unsigned char);

    map->map[oldOffset] = VISITED_SQUARE;
    map->map[newOffset] = PLAYER_SQUARE;
    map->posX = x;
    map->posY = y;
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

bool isMinotaurNearby(map_t *map) {
    bool res = FALSE;
    unsigned char posX = map->posX, posY = map->posY;
    int offset = posY * MAP_WIDTH * sizeof(unsigned char) + posX * sizeof(unsigned char), newOffset;
    int up, down, right, left, leftUp, leftDown, rightUp, rightDown;

    up = offset - MAP_WIDTH;
    down = offset + MAP_WIDTH;
    right = offset + 1;
    left = offset - 1;
    leftUp = up - 1;
    leftDown = down - 1;
    rightUp = up + 1;
    rightDown = down + 1;

    if ((up >= 0 && map->map[up] >= MINOTAUR_ID_BEGIN)
    || (down < MAP_WIDTH * MAP_HEIGHT && map->map[down] >= MINOTAUR_ID_BEGIN)
    || (right < MAP_WIDTH * MAP_HEIGHT && right % MAP_WIDTH != 0 && map->map[right] >= MINOTAUR_ID_BEGIN)
    || (left >= 0 && left % MAP_WIDTH != (MAP_WIDTH - 1) && map->map[left] >= MINOTAUR_ID_BEGIN)
    || (leftUp >= 0 && leftUp % MAP_WIDTH != (MAP_WIDTH - 1) && map->map[leftUp] >= MINOTAUR_ID_BEGIN)
    || (leftDown < MAP_WIDTH * MAP_HEIGHT && leftDown % MAP_WIDTH != (MAP_WIDTH - 1) && map->map[leftDown] >= MINOTAUR_ID_BEGIN)
    || (rightUp >= 0 && rightUp % MAP_WIDTH != 0 && map->map[rightUp] >= MINOTAUR_ID_BEGIN)
    || (rightDown < MAP_WIDTH * MAP_HEIGHT && rightDown % MAP_WIDTH != 0 && map->map[rightDown] >= MINOTAUR_ID_BEGIN)) {
        /* Minotaur is on a square next to us */
        loseLife(map);
        map->map[offset] = VISITED_SQUARE;
        map->posX = X_COORDINATE_ENTRANCE;
        map->posY = Y_COORDINATE_ENTRANCE;
        newOffset = map->posY * MAP_WIDTH * sizeof(unsigned char) + map->posX * sizeof(unsigned char);
        map->map[newOffset] = PLAYER_SQUARE;

        res = TRUE;
    }

    return res;
}
