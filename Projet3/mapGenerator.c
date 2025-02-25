#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include "structures/constants.h"
#include "utils/memoryUtils.h"
#include "utils/fileUtils.h"
#include "utils/ncurses.h"

int main(int argc, char *argv[]) {
    unsigned char width, height;
    unsigned char difficulty;
    unsigned char *map;
    char path[MAX_PATH_LENGTH];
    char confirmation;
    int i;
    int j;
    int mapFd;
    int result;
    int nbObstacles = 0;

    if (argc == 1) {
        fprintf(stderr, "Usage: ./mapGenerator.out MAP_PATH DIFFICULTY [MAP_WIDTH] [MAP_HEIGHT]\n\n");
        fprintf(stderr, "MAP_PATH:\tFor example, maps/test.bin\n");
        fprintf(stderr, "DIFFICULTY:\tBetween %d and %d\n", MIN_DIFFICULTY, MAX_DIFFICULTY);
        fprintf(stderr, "MAP_WIDTH:\tBetween %d and %d\n", MIN_WIDTH, MAX_WIDTH);
        fprintf(stderr, "MAP_HEIGHT:\tBetween %d and %d\n", MIN_HEIGHT, MAX_HEIGHT);
        exit(EXIT_FAILURE);
    }

    srand((unsigned int) (time(NULL) + getpid()));

    for (i = 0; argv[1][i] != '\0'; ++i) {
        path[i] = argv[1][i];
    }

    path[i] = '\0';

    if (argc > 2) {
        difficulty = (unsigned char) atoi(argv[2]);

        if (difficulty > MAX_DIFFICULTY)
            difficulty = MAX_DIFFICULTY;

        if (argc >= 4)
            width = atoi(argv[3]);
        else
            width = (unsigned char) ((unsigned char) rand() % (MAX_WIDTH + 1 - MIN_WIDTH) + MIN_WIDTH);

        if (argc >= 5)
            height = atoi(argv[4]);
        else
            height = (unsigned char) ((unsigned char) rand() % (MAX_HEIGHT + 1 - MIN_HEIGHT) + MIN_HEIGHT);
    }

    else {
        difficulty = (unsigned char) ((unsigned char) rand() % (MAX_DIFFICULTY + 1 - MIN_DIFFICULTY) + MIN_DIFFICULTY);
        width = (unsigned char) ((unsigned char) rand() % (MAX_WIDTH + 1 - MIN_WIDTH) + MIN_WIDTH);
        height = (unsigned char) ((unsigned char) rand() % (MAX_HEIGHT + 1 - MIN_HEIGHT) + MIN_HEIGHT);
    }

    /* Check if save file already exists */
    if ((mapFd = open(path, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1) {
        if (errno != EEXIST) {
            stop_ncurses();
            perror("An error occurred while trying to open the file");
            exit(EXIT_FAILURE);
        }

        fprintf(stderr, "DEBUG | The file specified (%s) already exists, do you want to overwrite it? (Y/N) ", path);

        if (scanf("%c", &confirmation) < 1) {
            stop_ncurses();
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }

        if (confirmation == 'Y' || confirmation == 'y') {
            mapFd = openFile(path, O_RDWR | O_TRUNC);
        }

        else {
            exit(EXIT_FAILURE);
        }
    }

    map = (unsigned char*) malloc_check(sizeof(unsigned char) * height * width);

    for (i = 0; i < height; ++i) {
        for (j = 0; j < width; ++j) {
            result = rand() % (MAX_DIFFICULTY - difficulty + (rand() % difficulty + 1));
            map[i * width + j] = (unsigned char) (result == 0 ? OBSTACLE : EMPTY);
            nbObstacles = result == 0 ? nbObstacles + 1 : nbObstacles;
        }
    }

    writeFileOff(mapFd, &width, 0, SEEK_SET, sizeof(unsigned char));
    writeFile(mapFd, &height, sizeof(unsigned char));
    writeFile(mapFd, map, sizeof(unsigned char) * height * width);

    fprintf(stderr, "DEBUG | Generated a map with dimensions %dw %dh, added %d obstacles\n", width, height, nbObstacles);

    return EXIT_SUCCESS;
}