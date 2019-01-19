#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include "mapEditor.h"
#include "fileUtils.h"

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
int loadMap(char *mapName) {
    int fd;

    if ((fd = open(mapName, O_RDWR | O_CREAT | O_EXCL, S_IRWXU)) != -1) {
        /* Create empty map */
        printf("Creating new map file\n");
        initializeMap(fd);
    }

    else if (errno == EEXIST && (fd = open(mapName, O_RDWR, S_IRWXU)) != -1) {
        /* Map file already exists, reading it */
    }

    else {
        perror("An error occurred while trying to open a file");
        exit(EXIT_FAILURE);
    }

    return fd;
}

int setWall(int fd, unsigned char type, int x, int y) {
    /* Map version + number of lives */
    int initialPadding = sizeof(int) + sizeof(unsigned char);
    int offset, result = -1;

    switch(type) {
        case EMPTY_SQUARE:
        case INVISIBLE_WALL:
        case VISIBLE_WALL:
        case DISCOVERED_WALL:
            /* If the coordinates aren't corresponding to the entry or exit and are within the map's width and height, we write the type to the corresponding position */
            if ((x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) && ((x != X_POS_BEGINNING || y != Y_POS_BEGINNING) && (x != X_POS_END || y != Y_POS_END))) {
                offset = initialPadding + (y * MAP_WIDTH * sizeof(unsigned char) + x * sizeof(unsigned char));
                writeFileOff(fd, &type, offset, sizeof(unsigned char));
                result = 0;
            }

            break;

        default:
            printf("Unsupported wall type");
    }

    return result;
}
