#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <string.h>
#include "ncurses.h"
#include "constants.h"
#include "windowDrawer.h"
#include "mapEditor.h"
#include "fileUtils.h"
#include "gameManager.h"
#include "mapUtils.h"

int main(int argc, char *argv[]) {
    int i, fd, newSquare, remainingLives, cpt = 0;
    char filename[256];
    unsigned char posX, posY, origPosX, origPosY;
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
    initialize_ncurses();
    ncurses_mouse();
    ncurses_colors();

    clear();
    refresh();

    init_pair(PAIR_COLOR_PLAYER, COLOR_WHITE, COLOR_PLAYER);
    init_pair(PAIR_COLOR_VISIBLE_WALL, COLOR_VISIBLE_WALL, COLOR_VISIBLE_WALL);
    /*init_pair(PAIR_COLOR_INVISIBLE_WALL, COLOR_INVISIBLE_WALL, COLOR_INVISIBLE_WALL);*/

    /* To make invisible walls actually invisible */
    init_pair(PAIR_COLOR_INVISIBLE_WALL, COLOR_EMPTY_SQUARE, COLOR_EMPTY_SQUARE);
    init_pair(PAIR_COLOR_EMPTY_SQUARE, COLOR_EMPTY_SQUARE, COLOR_EMPTY_SQUARE);
    init_pair(PAIR_COLOR_PLUS_SIGN, COLOR_GREEN, COLOR_BLACK);
    init_pair(PAIR_COLOR_MINUS_SIGN, COLOR_RED, COLOR_BLACK);
    init_pair(PAIR_COLOR_DISCOVERED_WALL, COLOR_DISCOVERED_WALL, COLOR_DISCOVERED_WALL);
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

    fd = loadGame(filename);

    drawMap(gameWindow, fd);
    loadStateWindowGame(stateWindow);

    getPlayerPosition(fd, &posX, &posY);

    remainingLives = getRemainingLives(fd);

    if (remainingLives == 0) {
        printInMiddle(gameWindow, MAP_WIDTH, MAP_HEIGHT, GAME_OVER_LOST);
    }
    else if (posX == X_POS_END && posY == Y_POS_END) {
        printInMiddle(gameWindow, MAP_WIDTH, MAP_HEIGHT, GAME_OVER_WON);
    }

    posX *= SQUARE_WIDTH;
    /* Draw the player at the last saved position */
    drawSquare(gameWindow, PLAYER_SQUARE, posX, posY, true);
    updateMoves(stateWindow, fd);
    updateLivesLeft(stateWindow, fd);
    updateDiscoveredWalls(stateWindow, fd);

    while ((i = getch()) != KEY_F(2)) {
        origPosX = posX;
        origPosY = posY;

        if (remainingLives != 0 && (posX / SQUARE_WIDTH != X_POS_END || posY != Y_POS_END)) {
            /* Draw the visited square */
            drawSquare(gameWindow, VISITED_SQUARE, posX, posY, FALSE);

            switch(i) {
                case KEY_LEFT:
                    if (posX / SQUARE_WIDTH > 0)
                        posX -= SQUARE_WIDTH;
                    break;
                case KEY_RIGHT:
                    if (posX / SQUARE_WIDTH < MAP_WIDTH - 1)
                        posX += SQUARE_WIDTH;
                    break;
                case KEY_UP:
                    if (posY > 0)
                        posY--;
                    break;
                case KEY_DOWN:
                    if (posY < MAP_HEIGHT - 1)
                        posY++;
                    break;
            }

            newSquare = movePlayer(fd, posX / SQUARE_WIDTH, posY);

            switch (newSquare) {
                case VISITED_SQUARE:
                    if (posX / SQUARE_WIDTH == X_POS_END && posY == Y_POS_END) {
                        printInMiddle(gameWindow, MAP_WIDTH, MAP_HEIGHT, GAME_OVER_WON);
                    }

                    /* Player has moved, display the current square as the player's position/color */
                    drawSquare(gameWindow, PLAYER_SQUARE, posX, posY, TRUE);
                    updateMoves(stateWindow, fd);

                    break;

                case DISCOVERED_WALL:
                    /* Player has hit an invisible that has become "discovered", draw it as such */
                    drawSquare(gameWindow, DISCOVERED_WALL, posX, posY, TRUE);

                    updateLivesLeft(stateWindow, fd);
                    updateDiscoveredWalls(stateWindow, fd);

                    remainingLives = getRemainingLives(fd);

                    if (remainingLives == 0) {
                        printInMiddle(gameWindow, MAP_WIDTH, MAP_HEIGHT, GAME_OVER_LOST);
                    }

                    else {
                        if(cpt != 0)
                            wprintw(informationWindow, "\n");

                        wprintw(informationWindow, "Lost one life, %d remaining!", remainingLives);
                        wrefresh(informationWindow);

                        cpt++;
                    }

                case -1:
                    /* We either encountered a wall or there was an error, so simply set the position back to what it was before the move */
                    posX = origPosX;
                    posY = origPosY;

                    drawSquare(gameWindow, PLAYER_SQUARE, posX, posY, TRUE);

                    break;

                default:
                    break;
            }
        }

        wrefresh(gameWindow);
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
