#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int i;
    char filename[256], nbSeconds[11];
    char **newArgv;

    if (argc == 3) {
        for (i = 0; argv[1][i] != '\0'; ++i) {
            filename[i] = argv[1][i];
        }

        filename[i] = '\0';
        strcpy(nbSeconds, argv[2]);
    }

    else {
        printf("Please enter a program name to start: ");
        if (scanf("%[a-zA-Z0-9._-/]s", filename) != 1) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }

        while (((i = getchar()) != '\n') || (i == EOF));

        printf("Please enter a number of seconds to wait for: ");
        if (scanf("%10s", nbSeconds) != 1) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }
    }

    newArgv = (char**) malloc(sizeof(char*) * 2);
    newArgv[0] = filename;
    newArgv[1] = nbSeconds;

    if (execve(filename, newArgv, NULL) == -1) {
        perror("An error occurred while calling the executable");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_FAILURE);
}