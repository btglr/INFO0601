#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

int child(int i) {
    srand(time(NULL) + getpid());

    printf("Child %d, Number %d\n", i, rand());
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    int i, n;
    pid_t pid;

    if (argc == 2) {
        n = atoi(argv[1]);
    }

    else {
        if (scanf("%d", &n) != 1) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }
    }

    for(i = 1; i <= n; i++) {
        if ((pid = fork()) == -1) {
            fprintf(stderr, "An error occurred while creating the child %d\n", i);
            perror("Error ");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            child(i);
        }
    }

    printf("Created %d children...\n", n);
    sleep(2);
    printf("Finished execution.\n");

    return EXIT_SUCCESS;
}