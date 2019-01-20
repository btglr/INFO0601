#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include "mapEditor.h"
#include "fileUtils.h"
#include "ncursesUtils.h"
#include "ncurses.h"
#include "windowDrawer.h"
#include "constants.h"

int main(int argc, char *argv[]) {
    int i, fd, mouseX, mouseY, type, relativeXPosition, cpt = 0;
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

    init_pair(PAIR_COLOR_PLAYER, COLOR_PLAYER, COLOR_PLAYER);
    init_pair(PAIR_COLOR_VISIBLE_WALL, COLOR_VISIBLE_WALL, COLOR_VISIBLE_WALL);
    init_pair(PAIR_COLOR_INVISIBLE_WALL, COLOR_INVISIBLE_WALL, COLOR_INVISIBLE_WALL);
    init_pair(PAIR_COLOR_EMPTY_SQUARE, COLOR_EMPTY_SQUARE, COLOR_EMPTY_SQUARE);
    init_pair(PAIR_COLOR_PLUS_SIGN, COLOR_GREEN, COLOR_BLACK);
    init_pair(PAIR_COLOR_MINUS_SIGN, COLOR_RED, COLOR_BLACK);
    init_pair(PAIR_COLOR_VISITED_SQUARE, COLOR_VISITED_SQUARE, COLOR_VISITED_SQUARE);

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

    drawMap(gameWindow, fd);
    loadStateWindow(stateWindow, fd);

    while ((i = getch()) != KEY_F(2)) {
        if ((i == KEY_MOUSE) && (souris_getpos(&mouseX, &mouseY, NULL) == OK)) {
            /* If not false, the click was in the window and the new coordinates are in the mouseY and mouseX */
            if(wmouse_trafo(gameWindow, &mouseY, &mouseX, FALSE) != FALSE) {
                /* Get the relative x position (depends on SQUARE_WIDTH) */
                relativeXPosition = mouseX / SQUARE_WIDTH;

                if(cpt != 0)
                    wprintw(informationWindow, "\n");

                type = changeWall(fd, relativeXPosition, mouseY);

                if(type >= 0) {
                    wprintw(informationWindow, "Changed a wall at (%d, %d)", relativeXPosition, mouseY);
                    wrefresh(informationWindow);
                    drawWall(gameWindow, type, relativeXPosition * SQUARE_WIDTH, mouseY, true);

                    if(type == EMPTY_SQUARE || type == INVISIBLE_WALL) {
                        updateWallCount(stateWindow, fd);
                    }

                    cpt++;
                }
            }

            /* Click was in the state window */
            else if(wmouse_trafo(stateWindow, &mouseY, &mouseX, FALSE) != FALSE) {
                if(cpt != 0)
                    wprintw(informationWindow, "\n");

                if (mouseX >= PLUS_SIGN_POS_X && mouseX <= PLUS_SIGN_POS_X + 2 && mouseY == PLUS_SIGN_POS_Y) {
                    wprintw(informationWindow, "Added a life");
                    increaseLives(fd);
                }

                else if (mouseX >= MINUS_SIGN_POS_X && mouseX <= MINUS_SIGN_POS_X + 2 && mouseY == MINUS_SIGN_POS_Y) {
                    wprintw(informationWindow, "Removed a life");
                    decreaseLives(fd);
                }

                wrefresh(informationWindow);
                updateLivesCount(stateWindow, fd);

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