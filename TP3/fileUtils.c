#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "fileUtils.h"
#include "mapEditor.h"

/**
 * Seeks into a file and sets the cursor to the specified offset
 * @param fd A file descriptor
 * @param offset The offset which we want to seek to
 * @param from Where to read the file from
 */
void seekFile(int fd, off_t offset, int from) {
    if (lseek(fd, offset, from) == -1) {
        perror("An error occurred while seeking the file");
        exit(EXIT_FAILURE);
    }
}

/**
 * Writes into a file
 * @param fd A file descriptor
 * @param buf The buffer containing the data to write
 * @param length The length of the data to write
 */
ssize_t writeFile(int fd, void *buf, size_t length) {
    ssize_t bytesWritten;

    if ((bytesWritten = write(fd, buf, length)) == -1) {
        perror("An error occurred while writing the file");
        exit(EXIT_FAILURE);
    }

    return bytesWritten;
}

/**
 * Writes into a file starting from a specified offset
 * @param fd A file descriptor
 * @param buf The buffer containing the data to write
 * @param offset The offset which we want to seek to
 * @param length The length of the data to write
 */
ssize_t writeFileOff(int fd, void *buf, off_t offset, size_t length) {
    seekFile(fd, offset, SEEK_SET);
    return writeFile(fd, buf, length);
}

/**
 * Reads a file
 * @param fd A file descriptor
 * @param buf The buffer into which to read data
 * @param length The length of the data to read
 * @return The number of bytes successfully read
 */
ssize_t readFile(int fd, void *buf, size_t length) {
    ssize_t bytesRead;

    if ((bytesRead = read(fd, buf, length)) == -1) {
        perror("An error occurred while reading the file");
        exit(EXIT_FAILURE);
    }

    else if(bytesRead == 0) {
        fprintf(stderr, "An error occurred while reading the file: no bytes could be read\n");
        exit(EXIT_FAILURE);
    }

    return bytesRead;
}

/**
 * Reads a file starting from a specified offset
 * @param fd A file descriptor
 * @param buf The buffer into which to read data
 * @param offset The offset which we want to seek to
 * @param length The length of the data to read
 * @return The number of bytes successfully read
 */
ssize_t readFileOff(int fd, void *buf, off_t offset, size_t length) {
    seekFile(fd, offset, SEEK_SET);
    return readFile(fd, buf, length);
}

/*void test(int fd) {
    int i, mapVersion;
    unsigned char lives;
    unsigned char buffer[MAP_WIDTH * MAP_HEIGHT];

    *//* Temp, reads the map's content and displays it *//*

    seekFile(fd, 0, SEEK_SET);
    readFile(fd, &mapVersion, sizeof(int));
    readFile(fd, &lives, sizeof(unsigned char));
    readFile(fd, buffer, sizeof(unsigned char) * MAP_WIDTH * MAP_HEIGHT);

    printf("Map version: %d\n", mapVersion);
    printf("Number of lives: %d\n", lives);

    for(i = 0; i < MAP_WIDTH * MAP_HEIGHT; ++i) {
        if(i % 30 == 0) {
            printf("\n");
        }

        printf("%d ", buffer[i]);
    }
    printf("\n");
}*/

/**
 * Closes a file
 * @param fd A file descriptor
 */
void closeFile(int fd) {
    if (close(fd) == -1) {
        perror("An error occurred while closing the file");
        exit(EXIT_FAILURE);
    }
}
