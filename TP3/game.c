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

int main(int argc, char *argv[]) {
    int i, fd, origPosX, origPosY, posX, posY, newSquare, remainingLives, cpt = 0;
    char *gameOver = GAME_OVER;
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
    loadStateWindowManager(stateWindow, fd);

    posX = X_POS_BEGINNING;
    posY = Y_POS_BEGINNING;

    while ((i = getch()) != KEY_F(2)) {
        origPosX = posX;
        origPosY = posY;

        wattron(gameWindow, COLOR_PAIR(PAIR_COLOR_VISITED_SQUARE));
        mvwprintw(gameWindow, posY, posX, "  ");
        wattroff(gameWindow, COLOR_PAIR(PAIR_COLOR_VISITED_SQUARE));

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
                /* Player has moved, display the current square as the player's position/color */
                drawWall(gameWindow, PLAYER_SQUARE, posX, posY, true);
                updateStateWindowManager(stateWindow, 1, 1, "Moves: ", getVisitedSquares(fd));

                break;

            case DISCOVERED_WALL:
                /* Player has hit an invisible that has become "discovered", draw it as such */
                drawWall(gameWindow, DISCOVERED_WALL, posX, posY, true);
                updateStateWindowManager(stateWindow, 1, 2, "Lives: ", getRemainingLives(fd));
                updateStateWindowManager(stateWindow, 1, 3, "Walls: ", getWallCount(fd));

                remainingLives = getRemainingLives(fd);

                if (remainingLives == 0) {
                    printInMiddle(gameWindow, MAP_WIDTH, MAP_HEIGHT, gameOver);
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

                wattron(gameWindow, COLOR_PAIR(PAIR_COLOR_PLAYER));
                mvwprintw(gameWindow, posY, posX, "  ");
                wattroff(gameWindow, COLOR_PAIR(PAIR_COLOR_PLAYER));
                wrefresh(gameWindow);

                break;

            default:
                break;
        }
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
