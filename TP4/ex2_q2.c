#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

int child(int id, int *f_c) {
    char message[512];

    if (close(f_c[PIPE_WRITE]) == -1) {
        perror("Child: failed to close the pipe in writing mode");
        exit(EXIT_FAILURE);
    }

    do {
        if (read(f_c[PIPE_READ], message, 512) == -1) {
            perror("Child: failed to read data from the pipe");
            exit(EXIT_FAILURE);
        }

        printf("Child %d received message: %s\n", id, message);
    } while (strcmp(message, "STOP") != 0);

    free(f_c);

    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    int nbChild, i, j, idChild, c;
    int **pipes;
    pid_t *pidArr;
    char buffer[512];

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

    if (nbChild < 1) {
        fprintf(stderr, "Number of children to spawn can't be less than 1\n");
        exit(EXIT_FAILURE);
    }

    pipes = (int**) malloc(nbChild * sizeof(int*));

    if (pipes == NULL) {
        perror("An error occurred while allocating memory");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < nbChild; ++i) {
        pipes[i] = (int*) malloc(2 * sizeof(int));

        if (pipes[i] == NULL) {
            perror("An error occurred while allocating memory");
            exit(EXIT_FAILURE);
        }

        if (pipe(pipes[i]) == -1) {
            perror("An error occurred while creating a pipe");
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
            for (j = 0; j < nbChild; ++j) {
                if (i != j) {
                    if (close(pipes[j][PIPE_WRITE]) == -1) {
                        perror("Child: failed to close the pipe in writing mode");
                        exit(EXIT_FAILURE);
                    }

                    if (close(pipes[j][PIPE_READ]) == -1) {
                        perror("Child: failed to close the pipe in reading mode");
                        printf("i: %d, j: %d\n", i, j);
                        exit(EXIT_FAILURE);
                    }

                    free(pipes[j]);
                }
            }

            free(pidArr);
            child(i, pipes[i]);
        }
    }

    do {
        printf("Enter the child's id: ");

        if (scanf("%d", &idChild) != 1) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }

        if (idChild == -1 || idChild >= nbChild || idChild < 0) {
            for (i = 0; i < nbChild; ++i) {
                if (write(pipes[i][PIPE_WRITE], "STOP", 512) == -1) {
                    perror("Child: failed to write data in the pipe");
                    exit(EXIT_FAILURE);
                }
            }
        }

        else {
            while (((c = getchar()) != '\n') || (c == EOF));
            printf("Enter the message to send: ");

            if (scanf("%[^\n]511s", buffer) != 1) {
                fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
                exit(EXIT_FAILURE);
            }

            if (strlen(buffer) > 0 && write(pipes[idChild][PIPE_WRITE], buffer, 512) == -1) {
                perror("Child: failed to write data in the pipe");
                exit(EXIT_FAILURE);
            }

            sleep(1);
        }
    } while (idChild > -1 && idChild < nbChild);

    for (i = 0; i < nbChild; ++i) {
        free(pipes[i]);
    }

    free(pipes);
    free(pidArr);

    return EXIT_SUCCESS;
}