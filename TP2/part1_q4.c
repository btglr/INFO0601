#include <fcntl.h>  /* Pour open */
#include <stdio.h>  /* Pour printf, scanf */
#include <stdlib.h> /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <string.h>
#include <sys/stat.h>  /* Pour O_WRONLY, O_CREAT, S_IRUSR, S_IWUSR */
#include <sys/types.h> /* Pour off_t */
#include <unistd.h>    /* Pour write */

int main(int argc, char* argv[]) {
    int i, fd, bytesRead, length = 0;
    char fileName[256], buffer[256], c;

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

        while (((c = getchar()) != '\n') || (c == EOF));
    }

    if ((fd = open(fileName, O_RDWR | O_CREAT | O_APPEND, S_IRWXU)) == -1) {
        perror("An error occurred while trying to open a file");
        exit(EXIT_FAILURE);
    }

    printf("Please enter a string to append to the end of the file: ");

    if (scanf("%254[^\n]s", buffer) != 1) {
        fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
        exit(EXIT_FAILURE);
    }

    length = strlen(buffer) + 1;

    if (write(fd, &length, sizeof(int)) == -1) {
        perror("An error occurred while writing the file");
        exit(EXIT_FAILURE);
    }

    if (write(fd, buffer, sizeof(char) * length) == -1) {
        perror("An error occurred while writing the file");
        exit(EXIT_FAILURE);
    }

    /* Set the cursor back to the beginning of the file */
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("An error occurred while seeking the file");
        exit(EXIT_FAILURE);
    }

    while ((bytesRead = read(fd, &length, sizeof(int))) > 0) {
        if ((bytesRead = read(fd, buffer, length)) > 0) {
            printf("%s\n", buffer);
        }

        else {
            perror("An error occurred while reading the file");
            exit(EXIT_FAILURE);
        }
    } 

    if (bytesRead == -1) {
        perror("An error occurred while reading the file");
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1) {
        perror("An error occurred while closing the file");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}