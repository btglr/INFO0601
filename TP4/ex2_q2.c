#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

int child(int id, int *f_c, int *c_f) {
    char message[512];

    if (close(f_c[PIPE_WRITE]) == -1) {
        perror("Child: failed to close the father to child pipe in writing mode");
        exit(EXIT_FAILURE);
    }

    if (close(c_f[PIPE_READ]) == -1) {
        perror("Child: failed to close the child to father pipe in reading mode");
        exit(EXIT_FAILURE);
    }

    do {
        if (read(f_c[PIPE_READ], message, 512) == -1) {
            perror("Child: failed to read data from the father to child pipe");
            exit(EXIT_FAILURE);
        }

        printf("Child %d received message: %s\n", id, message);

        sprintf(message, "%d received message", id);
        if (write(c_f[PIPE_WRITE], message, 512) == -1) {
            perror("Child: failed to write data in the child to father pipe");
            exit(EXIT_FAILURE);
        }
    } while (strcmp(message, "STOP") != 0);

    free(f_c);

    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    int nbChildren, i, j, idChild, c;
    int **f_c, **c_f;
    pid_t *pidArr;
    char buffer[512];

    if (argc == 2) {
        nbChildren = atoi(argv[1]);
    }

    else {
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

    f_c = (int**) malloc(nbChildren * sizeof(int*));
    c_f = (int**) malloc(nbChildren * sizeof(int*));

    if (f_c == NULL) {
        perror("An error occurred while allocating memory");
        exit(EXIT_FAILURE);
    }

    if (c_f == NULL) {
        perror("An error occurred while allocating memory");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < nbChildren; ++i) {
        f_c[i] = (int*) malloc(2 * sizeof(int));
        c_f[i] = (int*) malloc(2 * sizeof(int));

        if (f_c[i] == NULL) {
            perror("An error occurred while allocating memory");
            exit(EXIT_FAILURE);
        }

        if (pipe(f_c[i]) == -1) {
            perror("An error occurred while creating a father to child pipe");
            exit(EXIT_FAILURE);
        }

        if (c_f[i] == NULL) {
            perror("An error occurred while allocating memory");
            exit(EXIT_FAILURE);
        }

        if (pipe(c_f[i]) == -1) {
            perror("An error occurred while creating a child to father pipe");
            exit(EXIT_FAILURE);
        }
    }

    pidArr = (pid_t*) malloc(nbChildren * sizeof(pid_t));

    for (i = 0; i < nbChildren; i++) {
        if ((pidArr[i] = fork()) == -1) {
            fprintf(stderr, "An error occurred while creating the child %d\n", i);
            perror("Error ");
            exit(EXIT_FAILURE);
        }

        if (pidArr[i] == 0) {
            for (j = 0; j < nbChildren; ++j) {
                if (i != j) {
                    if (close(f_c[j][PIPE_WRITE]) == -1) {
                        perror("Father to child: failed to close the pipe in writing mode");
                        exit(EXIT_FAILURE);
                    }

                    if (close(f_c[j][PIPE_READ]) == -1) {
                        perror("Father to child: failed to close the pipe in reading mode");
                        exit(EXIT_FAILURE);
                    }

                    if (close(c_f[j][PIPE_WRITE]) == -1) {
                        perror("Child to father: failed to close the pipe in writing mode");
                        exit(EXIT_FAILURE);
                    }

                    if (close(c_f[j][PIPE_READ]) == -1) {
                        perror("Child to father: failed to close the pipe in reading mode");
                        exit(EXIT_FAILURE);
                    }

                    free(f_c[j]);
                    free(c_f[j]);
                }
            }

            free(pidArr);
            child(i, f_c[i], c_f[i]);
        }
    }

    do {
        printf("Enter the child's id: ");

        if (scanf("%d", &idChild) != 1) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }

        if (idChild == -1 || idChild >= nbChildren || idChild < 0) {
            for (i = 0; i < nbChildren; ++i) {
                if (write(f_c[i][PIPE_WRITE], "STOP", 512) == -1) {
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

            if (strlen(buffer) > 0 && write(f_c[idChild][PIPE_WRITE], buffer, 512) == -1) {
                perror("Child: failed to write data in the pipe");
                exit(EXIT_FAILURE);
            }

            if (read(c_f[idChild][PIPE_READ], buffer, 512) == -1) {
                perror("Child: failed to read data from the pipe");
                exit(EXIT_FAILURE);
            }

            printf("Received response from child: %s\n", buffer);

            sleep(1);
        }
    } while (idChild > -1 && idChild < nbChildren);

    for (i = 0; i < nbChildren; ++i) {
        if (close(f_c[i][PIPE_WRITE]) == -1) {
            perror("Failed to close the father to child pipe in writing mode");
            exit(EXIT_FAILURE);
        }

        if (close(c_f[i][PIPE_READ]) == -1) {
            perror("Failed to close the child to father pipe in reading mode");
            exit(EXIT_FAILURE);
        }

        free(f_c[i]);
        free(c_f[i]);
    }

    free(f_c);
    free(c_f);
    free(pidArr);

    return EXIT_SUCCESS;
}