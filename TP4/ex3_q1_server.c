#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int nbPipes, i, idPipe, c;
    int *fd;
    char name[64], buffer[512];

    if (argc == 2) {
        nbPipes = atoi(argv[1]);
    } else {
        printf("Enter number of pipes to create: ");

        if (scanf("%d", &nbPipes) != 1) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }
    }

    if (nbPipes < 1) {
        fprintf(stderr, "Number of pipes to create can't be less than 1\n");
        exit(EXIT_FAILURE);
    }

    fd = (int*) malloc(sizeof(int) * nbPipes);

    if (fd == NULL) {
        perror("An error occurred while allocating memory");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < nbPipes; ++i) {
        sprintf(name, "/tmp/toto%d", (i + 1));

        if (mkfifo(name, S_IRUSR | S_IWUSR) == -1) {
            if(errno != EEXIST) {
                fprintf(stderr, "An error occurred while creating the pipe %s\n", name);
                perror(" ");
                exit(EXIT_FAILURE);
            }

            else {
                fprintf(stderr, "The pipe %s already exists\n", name);
            }
        }
    }

    for (i = 0; i < nbPipes; ++i) {
        sprintf(name, "/tmp/toto%d", (i + 1));

        if((fd[i] = open(name, O_WRONLY)) == -1) {
            fprintf(stderr, "An error occurred while opening the pipe %s\n", name);
            perror(" ");
            exit(EXIT_FAILURE);
        }
    }

    do {
        printf("Enter the pipe's id: ");

        if (scanf("%d", &idPipe) != 1) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }

        if (idPipe == -1 || idPipe > nbPipes || idPipe <= 0) {
            for (i = 0; i < nbPipes; ++i) {
                if (write(fd[i], "STOP", 512) == -1) {
                    perror("Failed to write data in the pipe");
                    exit(EXIT_FAILURE);
                }
            }
        }

        else {
            while (((c = getchar()) != '\n') || (c == EOF));
            printf("Enter the message to send: ");

            if (scanf("%511[^\n]s", buffer) != 1) {
                fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
                exit(EXIT_FAILURE);
            }

            if (strlen(buffer) > 0 && write(fd[idPipe - 1], buffer, 512) == -1) {
                perror("An error occurred while writing data into the pipe");
                exit(EXIT_FAILURE);
            }

            sleep(1);
        }
    } while (idPipe > -1 && idPipe <= nbPipes);

    for (i = 0; i < nbPipes; ++i) {
        if(close(fd[i]) == -1) {
            perror("An error occurred while closing the pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < nbPipes; ++i) {
        sprintf(name, "/tmp/toto%d", (i + 1));

        if(unlink(name) == -1) {
            if(errno != ENOENT) {
                fprintf(stderr, "An error occurred while removing the pipe %s", name);
                perror(" ");
                exit(EXIT_FAILURE);
            }
        }
    }


    return EXIT_SUCCESS;
}