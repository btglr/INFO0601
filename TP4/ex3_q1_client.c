#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int fd, idPipe;
    char name[64], buffer[512];

    if (argc == 2) {
        idPipe = atoi(argv[1]);
    } else {
        printf("Enter the id of the pipe to open: ");

        if (scanf("%d", &idPipe) != 1) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }
    }

    if (idPipe < 1) {
        fprintf(stderr, "Id of the pipe to open can't be less than 1\n");
        exit(EXIT_FAILURE);
    }

    sprintf(name, "/tmp/toto%d", idPipe);

    if((fd = open(name, O_RDONLY)) == -1) {
        fprintf(stderr, "An error occurred while opening the pipe %s\n", name);
        perror("");
        exit(EXIT_FAILURE);
    }

    do {
        if(read(fd, buffer, 512) == -1) {
            fprintf(stderr, "An error occurred while reading from the pipe %s\n", name);
            perror("");
            exit(EXIT_FAILURE);
        }

        printf("Read: %s\n", buffer);
    } while (strcmp(buffer, "STOP") != 0);

    if(close(fd) == -1) {
        perror("An error occurred while closing the pipe");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
