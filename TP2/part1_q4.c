#include <fcntl.h>     /* Pour open */
#include <stdio.h>     /* Pour printf, scanf */
#include <stdlib.h>    /* Pour exit, EXIT_SUCCESS, EXIT_FAILURE */
#include <sys/stat.h>  /* Pour O_WRONLY, O_CREAT, S_IRUSR, S_IWUSR */
#include <sys/types.h> /* Pour off_t */
#include <unistd.h>    /* Pour write */

int main(int argc, char* argv[]) {
    int i;
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
}