#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int nbSeconds;

    if (argc == 2) {
        nbSeconds = atoi(argv[1]);
    }

    else {
        printf("Please enter a number of seconds to wait for: ");

        if (scanf("%d", &nbSeconds) != 1) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("Hello, I'll wait %d seconds\n", nbSeconds);
    sleep(nbSeconds);

    return EXIT_SUCCESS;
}