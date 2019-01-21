#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "gameManager.h"
#include "fileUtils.h"
#include "constants.h"
#include "ncurses.h"

int loadGame(char *filename) {
    int fd, mapVersion;
    char saveFileName[MAX_FILENAME_LENGTH];
    char *originalFilename, *mapName;

    if (strstr(filename, "_game.bin")) {
        /* User specified a save file, load it if it exists */

        if ((fd = open(filename, O_RDONLY, S_IRUSR)) == -1) {
            stop_ncurses();
            perror("An error occurred while trying to open the specified save file");
            exit(EXIT_FAILURE);
        }
    }

    else {
        /* User specified a map name, we create a new save file */

        /* We attempt to open the map file */
        if ((fd = open(filename, O_RDONLY, S_IRUSR)) == -1) {
            stop_ncurses();
            perror("An error occurred while trying to open the specified map file");
            exit(EXIT_FAILURE);
        }

        originalFilename = (char*) malloc((strlen(filename) + 1) * sizeof(char));

        if(originalFilename == NULL) {
            stop_ncurses();
            fprintf(stderr, "An error occurred while trying to allocate memory\n");
            exit(EXIT_FAILURE);
        }

        strcpy(originalFilename, filename);

        /* Separate the map name from the extension */
        mapName = strtok(filename, ".");

        if(mapName == NULL) {
            stop_ncurses();
            fprintf(stderr, "An error occurred while trying to split string: delimiter not found\n");
            exit(EXIT_FAILURE);
        }

        readFileOff(fd, &mapVersion, 0, sizeof(int));
        sprintf(saveFileName, "%s_%d_game.bin", mapName, mapVersion);

        if ((fd = open(saveFileName, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR)) == -1) {
            stop_ncurses();
            perror("An error occurred while trying to open the new save file");
            exit(EXIT_FAILURE);
        }

        copyFile(originalFilename, saveFileName);

        free(originalFilename);
    }

    return fd;
}

/*
void loadStateWindow(WINDOW *window, int fd) {
    mvwprintw(window, 6, 1, "E ");
    mvwprintw(window, 6, 4, "Exit");

    wrefresh(window);
}

void updateStateWindow(WINDOW *window, int x, int y, char *s, int value) {
    size_t length = strlen(s);

    mvwprintw(window, y, x, s);

    */
/* Print additional spaces to clear any remaining numbers *//*

    mvwprintw(window, y, (int) (x + length), "%d  ", value);

    wrefresh(window);
}*/
