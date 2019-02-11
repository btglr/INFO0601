#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int nbChildren, i, j, idChild, c;
    int **f_c, **c_f;
    pid_t *pidArr;
    char buffer[512];

    if (argc == 2) {
        nbChildren = atoi(argv[1]);
    } else {
        printf("Enter number of children to spawn: ");

        if (scanf("%d", &nbChildren) != 1) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }
    }

    if (nbChildren < 1) {
        fprintf(stderr, "Number of children to spawn can't be less than 1\n");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}