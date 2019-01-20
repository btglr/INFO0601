#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include "mapEditor.h"
#include "fileUtils.h"
#include "ncursesUtils.h"
#include "ncurses.h"
#include "windowDrawer.h"

#define BORDER_WIDTH 2
#define BORDER_HEIGHT 2
#define BORDER_STATE_WINDOW_WIDTH 20 * SQUARE_WIDTH + BORDER_WIDTH
#define BORDER_STATE_WINDOW_HEIGHT 8 + BORDER_HEIGHT
#define BORDER_INFORMATION_WINDOW_WIDTH MAP_WIDTH * SQUARE_WIDTH + BORDER_STATE_WINDOW_WIDTH + BORDER_WIDTH
#define BORDER_INFORMATION_WINDOW_HEIGHT 4 + BORDER_HEIGHT
#define BORDER_GAME_WINDOW_WIDTH MAP_WIDTH * SQUARE_WIDTH + BORDER_WIDTH
#define BORDER_GAME_WINDOW_HEIGHT MAP_HEIGHT + BORDER_HEIGHT

int main(int argc, char *argv[]) {
    int i, fd, mouseX, mouseY, type, relativeXPosition, cpt = 0;
    unsigned char lives;
    /*int playerColor, discoveredWallColor, visibleWallColor, trailColor;*/
    char filename[256];
    WINDOW *borderInformationWindow, *informationWindow, *borderGameWindow, *gameWindow, *borderStateWindow, *stateWindow;

    if (argc == 2) {
        for (i = 0; argv[1][i] != '\0'; ++i) {
            filename[i] = argv[1][i];
        }

        filename[i] = '\0';
    }

    else {
        printf("Please enter a filename to open: ");

        if (scanf("%[a-zA-Z0-9._-/]s", filename) != 1) {
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
    addColor(COLOR_PLAYER, COLOR_PLAYER);
    addColor(COLOR_DISCOVERED_WALL, COLOR_DISCOVERED_WALL);
    addColor(COLOR_VISIBLE_WALL, COLOR_VISIBLE_WALL);
    addColor(COLOR_EMPTY_SQUARE, COLOR_EMPTY_SQUARE);
    addColor(COLOR_INVISIBLE_WALL, COLOR_INVISIBLE_WALL);
    addColor(COLOR_GREEN, COLOR_BLACK);
    addColor(COLOR_RED, COLOR_BLACK);

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
    mvwprintw(borderGameWindow, 0, 2, "Map Editor");
    mvwprintw(borderStateWindow, 0, 2, "State");

    wrefresh(borderInformationWindow);
    wrefresh(informationWindow);
    wrefresh(borderGameWindow);
    wrefresh(gameWindow);
    wrefresh(borderStateWindow);
    wrefresh(stateWindow);

    fd = loadMap(filename);

    readFileOff(fd, &lives, sizeof(int), sizeof(unsigned char));

    mvwprintw(stateWindow, 1, 1, "Lives: %d", lives);
    wattron(stateWindow, COLOR_PAIR(8));
    mvwaddch(stateWindow, 1, 12, ACS_HLINE | WA_BOLD);
    mvwaddch(stateWindow, 1, 13, ACS_PLUS | WA_BOLD);
    mvwaddch(stateWindow, 1, 14, ACS_HLINE | WA_BOLD);
    wattroff(stateWindow, COLOR_PAIR(8));
    wattron(stateWindow, COLOR_PAIR(9));
    mvwaddch(stateWindow, 1, 18, ACS_HLINE | WA_BOLD);
    mvwaddch(stateWindow, 1, 19, ACS_HLINE | WA_BOLD);
    mvwaddch(stateWindow, 1, 20, ACS_HLINE | WA_BOLD);
    wattroff(stateWindow, COLOR_PAIR(9));
    wrefresh(stateWindow);

    drawMap(gameWindow, fd);

    while ((i = getch()) != KEY_F(2)) {
        if ((i == KEY_MOUSE) && (souris_getpos(&mouseX, &mouseY, NULL) == OK)) {
            /* If not false, the click was in the window and the new coordinates are in the mouseY and mouseX */
            if(wmouse_trafo(gameWindow, &mouseY, &mouseX, FALSE) != FALSE) {
                /* Get the relative x position (depends on SQUARE_WIDTH) */
                relativeXPosition = mouseX / SQUARE_WIDTH;

                if(cpt != 0)
                    wprintw(informationWindow, "\n");

                type = changeWall(fd, relativeXPosition, mouseY);

                wprintw(informationWindow, "Changed a wall at (%d, %d)", relativeXPosition, mouseY);
                wrefresh(informationWindow);

                drawWall(gameWindow, type, relativeXPosition * SQUARE_WIDTH, mouseY, true);
                cpt++;
            }
        }
    }

    closeFile(fd);

    delwin(informationWindow);
    delwin(borderInformationWindow);
    delwin(gameWindow);
    delwin(borderGameWindow);

    /* Stopping ncurses */
    ncurses_stopper();

    return EXIT_SUCCESS;
}