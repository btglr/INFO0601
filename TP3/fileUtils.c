#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "fileUtils.h"
#include "ncurses.h"
#include <string.h>
#include <stdarg.h>

ssize_t copyFile(char *src, char *dest) {
    int fdOrig, fdDest;
    char buf[1024];
    ssize_t bytesRead, bytesWritten = 0;

    fdOrig = openFile(src, O_RDONLY);
    fdDest = openFile(dest, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    while ((bytesRead = readFile(fdOrig, buf, sizeof(buf))) > 0) {
        bytesWritten += writeFile(fdDest, buf, bytesRead);
    }

    closeFile(fdDest);
    closeFile(fdOrig);

    return bytesWritten;
}

/**
 * Opens a file with the given filename
 * @param filename The file name
 * @param flags The flags with which to open the file (O_RDWR, ...)
 * @param ... Optionally, the mode of the file (S_IRWXU, ...)
 * @return A file descriptor corresponding to the opened file
 */
int openFile(char *filename, int flags, ...) {
    va_list va;
    mode_t mode;
    int fd;

    va_start(va, flags);
    mode = va_arg(va, mode_t);

    if ((fd = open(filename, flags, mode)) == -1) {
        stop_ncurses();
        perror("An error occurred while trying to open a file");
        exit(EXIT_FAILURE);
    }

    va_end(va);

    return fd;
}

/**
 * Seeks into a file and sets the cursor to the specified offset
 * @param fd A file descriptor
 * @param offset The offset which we want to seek to
 * @param from Where to read the file from
 */
void seekFile(int fd, off_t offset, int from) {
    if (lseek(fd, offset, from) == -1) {
        stop_ncurses();
        perror("An error occurred while seeking the file");
        exit(EXIT_FAILURE);
    }
}

/**
 * Writes into a file
 * @param fd A file descriptor
 * @param buf The buffer containing the data to write
 * @param length The length of the data to write
 * @return The number of bytes successfully written
 */
ssize_t writeFile(int fd, void *buf, size_t length) {
    ssize_t bytesWritten;

    if ((bytesWritten = write(fd, buf, length)) == -1) {
        stop_ncurses();
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
 * @return The number of bytes successfully written
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
        stop_ncurses();
        perror("An error occurred while reading the file");
        exit(EXIT_FAILURE);
    }

    /*else if(bytesRead == 0) {
        fprintf(stderr, "An error occurred while reading the file: no bytes could be read\n");
        exit(EXIT_FAILURE);
    }*/

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

/**
 * Closes a file
 * @param fd A file descriptor
 */
void closeFile(int fd) {
    if (close(fd) == -1) {
        stop_ncurses();
        perror("An error occurred while closing the file");
        exit(EXIT_FAILURE);
    }
}
