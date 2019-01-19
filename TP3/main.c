#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include "mapEditor.h"
#include "fileUtils.h"
#include "ncursesUtils.h"
#include "ncurses.h"
#include "windowDrawer.h"

#define BORDER_STATE_WINDOW_WIDTH 20 * SQUARE_WIDTH
#define BORDER_STATE_WINDOW_HEIGHT 10
#define BORDER_INFORMATION_WINDOW_WIDTH MAP_WIDTH * SQUARE_WIDTH + BORDER_STATE_WINDOW_WIDTH
#define BORDER_INFORMATION_WINDOW_HEIGHT 6
#define BORDER_GAME_WINDOW_WIDTH MAP_WIDTH * SQUARE_WIDTH
#define BORDER_GAME_WINDOW_HEIGHT MAP_HEIGHT

int main(int argc, char *argv[]) {
    int i, fd, mouseX, mouseY, added, cpt = 0;
    /*int playerColor, discoveredWallColor, visibleWallColor, trailColor;*/
    char fileName[256];
    WINDOW *borderInformationWindow, *informationWindow, *borderGameWindow, *gameWindow, *borderStateWindow, *stateWindow;

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

    /* Initializing ncurses */
    ncurses_initialiser();
    ncurses_souris();
    ncurses_couleurs();

    clear();
    refresh();

/*    playerColor = addColor(COLOR_BLACK, COLOR_PLAYER);
    discoveredWallColor = addColor(COLOR_BLACK, COLOR_DISCOVERED_WALL);
    visibleWallColor = addColor(COLOR_BLACK, COLOR_VISIBLE_WALL);
    trailColor = addColor(COLOR_BLACK, COLOR_EMPTY_SQUARE);*/
    addColor(COLOR_BLACK, COLOR_PLAYER);
    addColor(COLOR_BLACK, COLOR_DISCOVERED_WALL);
    addColor(COLOR_BLACK, COLOR_VISIBLE_WALL);
    addColor(COLOR_BLACK, COLOR_EMPTY_SQUARE);

    borderInformationWindow = initializeWindow(
            BORDER_INFORMATION_WINDOW_WIDTH,
            BORDER_INFORMATION_WINDOW_HEIGHT,
            0,
            0);
    informationWindow = initializeSubWindow(borderInformationWindow,
            BORDER_INFORMATION_WINDOW_WIDTH - 2,
            BORDER_INFORMATION_WINDOW_HEIGHT - 2,
            1,
            1);
    borderGameWindow = initializeWindow(
            BORDER_GAME_WINDOW_WIDTH,
            BORDER_GAME_WINDOW_HEIGHT,
            0,
            BORDER_INFORMATION_WINDOW_HEIGHT);
    gameWindow = initializeSubWindow(borderGameWindow,
            BORDER_GAME_WINDOW_WIDTH - 2,
            BORDER_GAME_WINDOW_HEIGHT - 2,
            1,
            BORDER_INFORMATION_WINDOW_HEIGHT + 1);
    borderStateWindow = initializeWindow(
            BORDER_STATE_WINDOW_WIDTH,
            BORDER_STATE_WINDOW_HEIGHT,
            BORDER_GAME_WINDOW_WIDTH,
            BORDER_INFORMATION_WINDOW_HEIGHT);
    stateWindow = initializeSubWindow(borderStateWindow,
            BORDER_STATE_WINDOW_WIDTH - 2,
            BORDER_STATE_WINDOW_HEIGHT - 2,
            BORDER_GAME_WINDOW_WIDTH + 1,
            BORDER_INFORMATION_WINDOW_HEIGHT + 1);

    scrollok(informationWindow, true);

    box(borderInformationWindow, 0, 0);
    box(borderGameWindow, 0, 0);
    box(borderStateWindow, 0, 0);

    mvwprintw(borderInformationWindow, 0, 2, "Information");
    mvwprintw(borderGameWindow, 0, 2, "Game");
    mvwprintw(borderStateWindow, 0, 2, "State");

    wrefresh(borderInformationWindow);
    wrefresh(informationWindow);
    wrefresh(borderGameWindow);
    wrefresh(gameWindow);
    wrefresh(borderStateWindow);
    wrefresh(stateWindow);

    fd = loadMap(fileName);

    setWall(fd, VISIBLE_WALL, 5, 5);
    setWall(fd, VISIBLE_WALL, 0, 9);
    setWall(fd, VISIBLE_WALL, 6, 5);
    setWall(fd, VISIBLE_WALL, 7, 5);
    setWall(fd, DISCOVERED_WALL, 8, 5);

    drawMap(gameWindow, fd);

    while ((i = getch()) != KEY_F(2)) {
        if ((i == KEY_MOUSE) && (souris_getpos(&mouseX, &mouseY, NULL) == OK)) {
            if(wmouse_trafo(gameWindow, &mouseY, &mouseX, FALSE) != FALSE) {
                /* If not false, the click was in the window and the new coordinates are in the mouseY and mouseX */
                if(cpt != 0)
                    wprintw(informationWindow, "\n");

                added = setWall(fd, VISIBLE_WALL, mouseX, mouseY);

                if(added) {
                    wprintw(informationWindow, "Added a wall at (%d, %d)", mouseX, mouseY);
                    wrefresh(informationWindow);

                    drawSquare(gameWindow, VISIBLE_WALL, mouseX, mouseY, true);
                    cpt++;
                }
            }
        }
    }

    closeFile(fd);

    delwin(informationWindow);
    delwin(borderInformationWindow);
    delwin(gameWindow);
    delwin(borderGameWindow);

    /*fd = loadMap(fileName);

    setWall(fd, VISIBLE_WALL, 28, 8);

    test(fd);

    color = addColor(COLOR_BLACK, COLOR_BLUE);
    printf("Pair: %d\n", color);*/

    /* Stopping ncurses */
    ncurses_stopper();

    return EXIT_SUCCESS;
}