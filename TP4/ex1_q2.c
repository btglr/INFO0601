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

    exit(i);
}

int main(int argc, char *argv[]) {
    int i, nbChild, status;
    pid_t pid;

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

    for(i = 1; i <= nbChild; i++) {
        if((pid = fork()) == -1) {
            fprintf(stderr, "An error occurred while creating the child %d\n", i);
            perror("Error ");
            exit(EXIT_FAILURE);
        }

        if(pid == 0) {
            child(i);
        }
    }

    for(i = 0; i < nbChild; i++) {
        if((pid = wait(&status)) == -1) {
            perror("An error occurred while waiting for the child ");
            exit(EXIT_FAILURE);
        }

        printf("Id: %d, Pid: %d\n", WEXITSTATUS(status), pid);
    }

    printf("Created %d children...\n", nbChild);
    printf("Finished execution.\n");

    return EXIT_SUCCESS;
}