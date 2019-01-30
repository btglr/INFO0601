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

int openFile(char *folder, char *filename, int flags, ...) {
    va_list va;
    mode_t mode;
    int fd;

    char *path = getPath(folder, filename);

    va_start(va, flags);
    mode = va_arg(va, mode_t);

    if ((fd = open(path, flags, mode)) == -1) {
        stop_ncurses();
        perror("An error occurred while trying to open a file");
        exit(EXIT_FAILURE);
    }

    va_end(va);

    free(path);

    return fd;
}

void seekFile(int fd, off_t offset, int from) {
    if (lseek(fd, offset, from) == -1) {
        stop_ncurses();
        perror("An error occurred while seeking the file");
        exit(EXIT_FAILURE);
    }
}

ssize_t writeFile(int fd, void *buf, size_t length) {
    ssize_t bytesWritten;

    if ((bytesWritten = write(fd, buf, length)) == -1) {
        stop_ncurses();
        perror("An error occurred while writing the file");
        exit(EXIT_FAILURE);
    }

    return bytesWritten;
}

ssize_t writeFileOff(int fd, void *buf, off_t offset, int from, size_t length) {
    seekFile(fd, offset, from);
    return writeFile(fd, buf, length);
}

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

ssize_t readFileOff(int fd, void *buf, off_t offset, int from, size_t length) {
    seekFile(fd, offset, from);
    return readFile(fd, buf, length);
}

void closeFile(int fd) {
    if (close(fd) == -1) {
        stop_ncurses();
        perror("An error occurred while closing the file");
        exit(EXIT_FAILURE);
    }
}

char *getPath(char *folder, char *filename) {
    char *path = (char*) malloc((strlen(folder) + strlen(filename) + 2) * sizeof(char));

    if (path == NULL) {
        stop_ncurses();
        fprintf(stderr, "An error occurred while trying to allocate memory\n");
        exit(EXIT_FAILURE);
    }

    sprintf(path, "%s/%s", folder, filename);

    return path;
}
