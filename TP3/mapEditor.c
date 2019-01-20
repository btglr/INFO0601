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

    fd = open(mapName, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);

    if (fd == -1) {
        if(errno == EEXIST) {
            fd = openFile(mapName, O_RDWR);
        }

        else {
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

int changeWall(int fd, int x, int y) {
    /* Map version + number of lives */
    unsigned char type;
    int initialPadding = sizeof(int) + sizeof(unsigned char);
    int offset = initialPadding + (y * MAP_WIDTH * sizeof(unsigned char) + x * sizeof(unsigned char));

    readFileOff(fd, &type, offset, sizeof(unsigned char));

    switch(type) {
        case EMPTY_SQUARE:
            type = INVISIBLE_WALL;
            break;

        case INVISIBLE_WALL:
            type = VISIBLE_WALL;
            break;

        case VISIBLE_WALL:
            type = EMPTY_SQUARE;
            break;

        default:
            printf("Unsupported wall type");
    }

    /* If the coordinates aren't corresponding to the entry or exit and are within the map's width and height, we write the type to the corresponding position */
    if ((x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) && ((x != X_POS_BEGINNING || y != Y_POS_BEGINNING) && (x != X_POS_END || y != Y_POS_END))) {
        writeFileOff(fd, &type, offset, sizeof(unsigned char));
    }

    return type;
}

int setWall(int fd, unsigned char type, int x, int y) {
    /* Map version + number of lives */
    int initialPadding = sizeof(int) + sizeof(unsigned char);
    int offset;
    ssize_t bytesWritten = -1;

    switch(type) {
        case EMPTY_SQUARE:
        case INVISIBLE_WALL:
        case VISIBLE_WALL:
        case DISCOVERED_WALL:
            /* If the coordinates aren't corresponding to the entry or exit and are within the map's width and height, we write the type to the corresponding position */
            if ((x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) && ((x != X_POS_BEGINNING || y != Y_POS_BEGINNING) && (x != X_POS_END || y != Y_POS_END))) {
                offset = initialPadding + (y * MAP_WIDTH * sizeof(unsigned char) + x * sizeof(unsigned char));
                bytesWritten = writeFileOff(fd, &type, offset, sizeof(unsigned char));
            }

            break;

        default:
            printf("Unsupported wall type");
    }

    return (int) bytesWritten;
}
