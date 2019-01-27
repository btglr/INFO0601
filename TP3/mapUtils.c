#include <unistd.h>
#include "mapUtils.h"
#include "constants.h"
#include "fileUtils.h"
#include "mapEditor.h"
#include "gameManager.h"

unsigned char getTotalLives(int fd) {
    unsigned char totalLives;

    readFileOff(fd, &totalLives, sizeof(int), SEEK_SET, sizeof(unsigned char));

    return totalLives;
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

int getMapVersion(int fd) {
    int mapVersion;

    readFileOff(fd, &mapVersion, 0, SEEK_SET, sizeof(int));

    return mapVersion;
}

void increaseMapVersion(int fd) {
    int mapVersion = getMapVersion(fd);

    mapVersion++;
    writeFileOff(fd, &mapVersion, 0, SEEK_SET, sizeof(int));
}

void decreaseTotalLives(int fd) {
    unsigned char lives = getTotalLives(fd);

    if(lives > 0) {
        lives--;
        writeFileOff(fd, &lives, sizeof(int), SEEK_SET, sizeof(unsigned char));
    }
}

void increaseTotalLives(int fd) {
    unsigned char lives = getTotalLives(fd);

    if (lives < 255) {
        lives++;
        writeFileOff(fd, &lives, sizeof(int), SEEK_SET, sizeof(unsigned char));
    }
}

int getWallCount(int fd, int type) {
    int i, wallCount = 0;
    int initialPadding = sizeof(int) + sizeof(unsigned char);
    ssize_t bytesRead;
    unsigned char buffer[MAP_WIDTH * MAP_HEIGHT];

    bytesRead = readFileOff(fd, buffer, initialPadding, SEEK_SET, MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char));

    for(i = 0; i < bytesRead; ++i) {
        switch (type) {
            case INVISIBLE_WALL:
                wallCount = (buffer[i] == INVISIBLE_WALL) ? wallCount + 1 : wallCount;
                break;

            case DISCOVERED_WALL:
                wallCount = (buffer[i] == DISCOVERED_WALL) ? wallCount + 1 : wallCount;
                break;

            default:
                wallCount = (buffer[i] == VISIBLE_WALL || buffer[i] == INVISIBLE_WALL) ? wallCount + 1 : wallCount;
                break;
        }
    }

    return wallCount;
}

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

void setPlayerPosition(int fd, int x, int y) {
    unsigned char buf[2];
    int initialPadding = sizeof(int) + sizeof(unsigned char), offset;
    offset = initialPadding + (MAP_WIDTH * MAP_HEIGHT * sizeof(unsigned char)) + sizeof(unsigned char);

    buf[0] = (unsigned char) x;
    buf[1] = (unsigned char) y;

    writeFileOff(fd, buf, offset, SEEK_SET, 2 * sizeof(unsigned char));
}

unsigned char getWallAt(int fd, int x, int y) {
    int initialPadding = sizeof(int) + sizeof(unsigned char), offset;
    unsigned char wall = EMPTY_SQUARE;

    if ((x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)) {
        offset = initialPadding + (y * MAP_WIDTH * sizeof(unsigned char) + x * sizeof(unsigned char));
        readFileOff(fd, &wall, offset, SEEK_SET, sizeof(unsigned char));
    }

    return wall;
}

int makeMultipleOf(int v, int m) {
    return v - (v % m);
}

unsigned char getNextWallAt(int fd, int x, int y, int editor) {
    unsigned char originalType, nextWall = UNCHANGED;
    /* Map version + number of lives */
    int initialPadding = sizeof(int) + sizeof(unsigned char);
    int offset;
    ssize_t bytesRead;

    /* If the given coordinates are within the map */
    if ((x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)) {
        offset = initialPadding + (y * MAP_WIDTH * sizeof(unsigned char) + x * sizeof(unsigned char));

        bytesRead = readFileOff(fd, &originalType, offset, SEEK_SET, sizeof(unsigned char));

        if (bytesRead > 0) {
            if (editor) {
                nextWall = getNextWallEditor(originalType);
            }

            else {
                nextWall = getNextWallGame(originalType);
            }
        }
    }

    return nextWall;
}

unsigned char setWallAt(int fd, int x, int y, unsigned char type) {
    /* Map version + number of lives */
    int initialPadding = sizeof(int) + sizeof(unsigned char), offset;
    unsigned char res = UNCHANGED;
    ssize_t bytesWritten;

    /* If the given coordinates are within the map */
    if ((x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT)) {
        offset = initialPadding + (y * MAP_WIDTH * sizeof(unsigned char) + x * sizeof(unsigned char));

        /* If the coordinates aren't corresponding to the entry or exit we write the new wall to the corresponding position */
        if ((x != X_POS_BEGINNING || y != Y_POS_BEGINNING) && (x != X_POS_END || y != Y_POS_END)) {
            bytesWritten = writeFileOff(fd, &type, offset, SEEK_SET, sizeof(unsigned char));
            if (bytesWritten > 0) {
                res = type;
            }
        }
    }

    return res;
}
