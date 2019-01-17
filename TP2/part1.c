#include <fcntl.h>     /* Pour open */
#include <stdio.h>     /* Pour printf, scanf */
#include <stdlib.h>    /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/stat.h>  /* Pour O_WRONLY, O_CREAT, S_IRUSR, S_IWUSR */
#include <sys/types.h> /* Pour off_t */
#include <unistd.h>    /* Pour write */

int main(int argc, char* argv[]) {
    int i, fd;
    off_t filesize;
    char fileName[256];

    if (argc == 2) {
        for (i = 0; argv[1][i] != '\0'; ++i) {
            fileName[i] = argv[1][i];
        }

        fileName[i] = '\0';
    }

    else {
        printf("Please enter a filename to open: ");

        if (scanf("%[a-zA-Z0-9._-/]s", fileName) != 1) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }
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

    printf("File size: %ld octets\n", filesize);

    if (close(fd) == -1) {
        perror("An error occured while closing the file");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}