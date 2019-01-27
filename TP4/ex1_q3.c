#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <wait.h>

int child(int i) {
    int sleepTime;
    srand(time(NULL) + getpid());

    sleepTime = (rand() % 4) + 1;

    printf("Child %d (waiting %d sec)\n", i, sleepTime);
    sleep(sleepTime);

    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    int i, j, nbChild, status;
    pid_t *pidArr, pid;

    if (argc == 2) {
        nbChild = atoi(argv[1]);
    }

    else {
        printf("Enter number of children to spawn: ");

        if (scanf("%d", &nbChild) != 1) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }
    }

    pidArr = (pid_t*) malloc(nbChild * sizeof(pid_t));

    for (i = 0; i < nbChild; i++) {
        if ((pidArr[i] = fork()) == -1) {
            fprintf(stderr, "An error occurred while creating the child %d\n", i);
            perror("Error ");
            exit(EXIT_FAILURE);
        }

        if (pidArr[i] == 0) {
            free(pidArr);
            child(i);
        }
    }

    for (i = 0; i < nbChild; i++) {
        if ((pid = wait(&status)) == -1) {
            perror("An error occurred while waiting for the child ");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status)) {
            j = 0;
            while (pidArr[j] != pid) {
                j++;
            }

            printf("Id: %d, Pid: %d\n", j, pid);
        }

        else {
            fprintf(stderr, "The child exited unexpectedly\n");
        }
    }

    printf("Created %d children...\n", nbChild);
    printf("Finished execution.\n");

    return EXIT_SUCCESS;
}