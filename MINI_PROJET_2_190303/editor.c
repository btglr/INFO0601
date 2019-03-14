#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <string.h>
#include "mapEditor.h"
#include "fileUtils.h"
#include "ncurses.h"
#include "windowDrawer.h"
#include "constants.h"
#include "mapUtils.h"

int main(int argc, char *argv[]) {
    WINDOW *borderInformationWindow, *informationWindow, *borderGameWindow, *gameWindow, *borderStateWindow, *stateWindow;
    int i, ch, fd, mouseX, mouseY, mouseXPreviousClick = 0, mouseYPreviousClick = 0, dX, dY, relativeXPosition, cpt = 0, edited = FALSE, event, firstRightClick = TRUE;
    unsigned char wallType;
    char filename[MAX_FILENAME_LENGTH];

    if (argc == 2) {
        for (i = 0; argv[1][i] != '\0'; ++i) {
            filename[i] = argv[1][i];
        }

        filename[i] = '\0';
    }

    else {
        printf("Please enter a filename to open: ");

        if (fgets(filename, MAX_FILENAME_LENGTH, stdin) == NULL) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }

        strcpy(filename, strtok(filename, "\n"));
    }

    /* Initializing ncurses */
    initialize_ncurses();
    ncurses_mouse();
    ncurses_colors();

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

    fd = loadMapEditor(filename);

    drawMapFile(gameWindow, fd);
    loadStateWindowEditor(stateWindow, fd);

    while ((ch = getch()) != KEY_F(2)) {
        if ((ch == KEY_MOUSE) && (mouse_getpos(&mouseX, &mouseY, &event) == OK)) {
            /* If not false, the click was in the window and the new coordinates are in the mouseY and mouseX variables */
            if(wmouse_trafo(gameWindow, &mouseY, &mouseX, FALSE) != FALSE) {
                /* Get the relative x position (based on SQUARE_WIDTH) */
                relativeXPosition = mouseX / SQUARE_WIDTH;

                /* Right click can be used to set multiple walls at the same time: horizontally, vertically or as a corner */
                if (event & BUTTON3_CLICKED) {
                    wallType = getNextWallAt(fd, relativeXPosition, mouseY, TRUE);

                    if (firstRightClick) {
                        wallType = setWallAt(fd, relativeXPosition, mouseY, wallType);

                        if (wallType != UNCHANGED) {
                            drawSquare(gameWindow, wallType, makeMultipleOf(mouseX, SQUARE_WIDTH), mouseY, TRUE);
                        }

                        mouseXPreviousClick = makeMultipleOf(mouseX, SQUARE_WIDTH);
                        mouseYPreviousClick = mouseY;

                        edited = TRUE;
                        firstRightClick = FALSE;
                    }

                    else {
                        dX = makeMultipleOf(abs(mouseXPreviousClick - mouseX), SQUARE_WIDTH);
                        dY = abs(mouseYPreviousClick - mouseY);

                        while (dX > 0) {
                            mouseX = mouseXPreviousClick >= mouseX ? mouseXPreviousClick - dX : mouseXPreviousClick + dX;

                            wallType = changeWallEditor(fd, mouseX / SQUARE_WIDTH, mouseYPreviousClick);

                            if (wallType != UNCHANGED) {
                                drawSquare(gameWindow, wallType, mouseX, mouseYPreviousClick, TRUE);
                            }

                            dX -= SQUARE_WIDTH;
                            edited = TRUE;
                        }

                        while (dY > 0) {
                            mouseY = mouseYPreviousClick >= mouseY ? mouseYPreviousClick - dY : mouseYPreviousClick + dY;

                            wallType = changeWallEditor(fd, mouseXPreviousClick / SQUARE_WIDTH, mouseY);

                            if (wallType != UNCHANGED) {
                                drawSquare(gameWindow, wallType, mouseXPreviousClick, mouseY, true);
                            }

                            dY--;
                            edited = TRUE;
                        }

                        /* Reset the right click variable for the next one */
                        firstRightClick = TRUE;
                    }
                }

                /* Or with other mouse buttons, wall per wall */
                else {
                    /* Reset the right click (aka cancels the previous right click if the user then clicked left) */
                    firstRightClick = TRUE;
                    if(cpt != 0)
                        wprintw(informationWindow, "\n");

                    wallType = changeWallEditor(fd, relativeXPosition, mouseY);

                    if(wallType >= 0) {
                        if (wallType != UNCHANGED) {
                            wprintw(informationWindow, "Changed a wall at (%d, %d)", relativeXPosition, mouseY);
                            wrefresh(informationWindow);

                            drawSquare(gameWindow, wallType, relativeXPosition * SQUARE_WIDTH, mouseY, true);
                        }

                        cpt++;
                        edited = TRUE;
                    }
                }

                updateWallCount(stateWindow, fd);
            }

            /* Click was in the state window */
            else if(wmouse_trafo(stateWindow, &mouseY, &mouseX, FALSE) != FALSE) {
                if(cpt != 0)
                    wprintw(informationWindow, "\n");

                if (mouseX >= X_COORDINATE_PLUS_SIGN && mouseX <= X_COORDINATE_PLUS_SIGN + 2 && mouseY == Y_COORDINATE_PLUS_SIGN) {
                    wprintw(informationWindow, "Added a life");
                    increaseTotalLives(fd);
                    edited = TRUE;
                }

                else if (mouseX >= X_COORDINATE_MINUS_SIGN && mouseX <= X_COORDINATE_MINUS_SIGN + 2 && mouseY == Y_COORDINATE_MINUS_SIGN) {
                    wprintw(informationWindow, "Removed a life");
                    decreaseTotalLives(fd);
                    edited = TRUE;
                }

                wrefresh(informationWindow);
                updateLivesCount(stateWindow, fd);

                cpt++;
            }
        }
    }

    if (edited == 1) {
        increaseMapVersion(fd);
    }

    closeFile(fd);

    delwin(informationWindow);
    delwin(borderInformationWindow);
    delwin(gameWindow);
    delwin(borderGameWindow);
    delwin(stateWindow);
    delwin(borderStateWindow);

    /* Stopping ncurses */
    stop_ncurses();

    return EXIT_SUCCESS;
}