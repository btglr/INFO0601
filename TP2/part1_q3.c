#include <fcntl.h>     /* Pour open */
#include <stdio.h>     /* Pour printf, scanf */
#include <stdlib.h>    /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/stat.h>  /* Pour O_WRONLY, O_CREAT, S_IRUSR, S_IWUSR */
#include <sys/types.h> /* Pour off_t */
#include <unistd.h>    /* Pour write */

int main(int argc, char* argv[]) {
    int i, fd, bytesRead, startPosition, endPosition;
    off_t filesize;
    char fileName[256], c;
    unsigned char *buffer;

    if (argc == 4) {
        for (i = 0; argv[1][i] != '\0'; ++i) {
            fileName[i] = argv[1][i];
        }

        fileName[i] = '\0';

        startPosition = atoi(argv[2]);
        endPosition = atoi(argv[3]);

        if(startPosition > endPosition) {
            fprintf(stderr, "Start position is greater than end position\n");
            exit(EXIT_FAILURE);
        }
    }

    else {
        printf("Please enter a filename to open: ");
        if (scanf("%[a-zA-Z0-9._-/]s", fileName) != 1) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }

        while (((c = getchar()) != '\n') || (c == EOF));

        printf("Please enter the start and end position: ");
        if (scanf("%d %d", &startPosition, &endPosition) != 2) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }
    }

    if((buffer = (unsigned char*) malloc(sizeof(unsigned char) * (endPosition - startPosition))) == NULL) {
        fprintf(stderr, "An error occurred during malloc\n");
        exit(EXIT_FAILURE);
    }

    if ((fd = open(fileName, O_RDONLY)) == -1) {
        perror("An error occurred while trying to open a file");
        exit(EXIT_FAILURE);
    }

    else {
        printf("The file exists\n");
    }

    if ((filesize = lseek(fd, 0L, SEEK_END)) == -1) {
        perror("An error occurred while seeking the file");
        exit(EXIT_FAILURE);
    }

    if (endPosition > filesize) {
        fprintf(stderr, "End position is greater than file size\n");
        exit(EXIT_FAILURE);
    }

    printf("File size: %ld octets\n", filesize);
    if (lseek(fd, startPosition, SEEK_SET) == -1) {
        perror("An error occurred while seeking the file");
        exit(EXIT_FAILURE);
    }

    if ((bytesRead = read(fd, buffer, endPosition - startPosition)) > 0) {
        printf("Reading file from position %d to %d\n", startPosition, endPosition);

        for (i = 0; i < bytesRead; ++i) {
            printf("%d (%c) ", buffer[i], buffer[i]);
        }

        printf("\n");
    }

    if (bytesRead == -1) {
        perror("An error occurred while reading the file");
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1) {
        perror("An error occured while closing the file");
        exit(EXIT_FAILURE);
    }

    free(buffer);
    
    return EXIT_SUCCESS;
}