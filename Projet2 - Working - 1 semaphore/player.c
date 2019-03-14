#undef _GNU_SOURCE
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "ncurses.h"
#include "constants.h"
#include "windowDrawer.h"
#include "mapEditor.h"
#include "fileUtils.h"
#include "gameManager.h"
#include "mapUtils.h"
#include "messageQueueUtils.h"
#include "structures.h"
#include "semaphoreUtils.h"
#include "sharedMemoryUtils.h"

bool run = TRUE;

void handler(int sig) {
    if (sig == SIGINT) {
        run = FALSE;
    }
}

int main(int argc, char *argv[]) {
    int i, keyQueue, queueId, semId, shmId, cpt = 0;
    unsigned char posX, posY, previousPosX, previousPosY, newSquare;
    WINDOW *borderInformationWindow, *informationWindow, *borderGameWindow, *gameWindow, *borderStateWindow, *stateWindow;
    request_t request;
    response_t response;
    struct sigaction action;
    map_t *map;
    bool sent = FALSE;

    if (argc == 2) {
        keyQueue = atoi(argv[1]);
    }

    else {
        printf("Please enter the message queue key: ");

        if (scanf("%d", &keyQueue) != 1) {
            fprintf(stderr, "An error occurred while trying to read input from keyboard\n");
            exit(EXIT_FAILURE);
        }
    }

    sigemptyset(&action.sa_mask);
    action.sa_handler = handler;

    if (sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "An error occurred while catching the SIGINT signal\n");
        exit(EXIT_FAILURE);
    }

    /* Initializing ncurses */
    initialize_ncurses();
    ncurses_mouse();
    ncurses_colors();

    clear();
    refresh();

    init_pair(PAIR_COLOR_PLAYER, COLOR_WHITE, COLOR_PLAYER);
    init_pair(PAIR_COLOR_VISIBLE_WALL, COLOR_VISIBLE_WALL, COLOR_VISIBLE_WALL);

    /* To make invisible walls visible */
    /*init_pair(PAIR_COLOR_INVISIBLE_WALL, COLOR_INVISIBLE_WALL, COLOR_INVISIBLE_WALL);*/

    /* To make invisible walls actually invisible */
    init_pair(PAIR_COLOR_INVISIBLE_WALL, COLOR_EMPTY_SQUARE, COLOR_EMPTY_SQUARE);
    init_pair(PAIR_COLOR_EMPTY_SQUARE, COLOR_EMPTY_SQUARE, COLOR_EMPTY_SQUARE);
    init_pair(PAIR_COLOR_PLUS_SIGN, COLOR_GREEN, COLOR_BLACK);
    init_pair(PAIR_COLOR_MINUS_SIGN, COLOR_RED, COLOR_BLACK);
    init_pair(PAIR_COLOR_DISCOVERED_WALL, COLOR_DISCOVERED_WALL, COLOR_DISCOVERED_WALL);
    init_pair(PAIR_COLOR_VISITED_SQUARE, COLOR_VISITED_SQUARE, COLOR_VISITED_SQUARE);
    init_pair(PAIR_COLOR_MINOTAUR, COLOR_RED, COLOR_BLACK);

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

    /* Get the keys from the controller */

    request.type = GET_KEYS_TYPE;
    queueId = openMessageQueue(keyQueue);
    sendMessage(queueId, &request, sizeof(request_t), 0);
    receiveMessage(queueId, &response, sizeof(response_t), RESPONSE_TYPE, 0);

    semId = response.semId;
    shmId = response.shmId;

    /* P on the player semaphore to allow a single player */
    P(semId, SEM_PLAYER, 1);

    /* Send the connection request to the controller so it can register us */
    request.type = CONNECT_TYPE;
    request.message.connect.pid = getpid();
    request.message.connect.programType = PLAYER;
    sendMessage(queueId, &request, sizeof(request_t), 0);

    /* Attach the map */
    map = (map_t*) attachSegment(shmId, 0);

    /* Draw the map */
    P(semId, SEM_MAP, 1);
    drawMap(gameWindow, map);
    V(semId, SEM_MAP, 1);

    loadStateWindowGame(stateWindow);

    posX = map->posX;
    posY = map->posY;

    /*if (map->livesLeft == 0) {
        printInMiddle(gameWindow, MAP_WIDTH, MAP_HEIGHT, GAME_OVER_LOST);
        wrefresh(gameWindow);
    }

    else if (posX == X_COORDINATE_EXIT && posY == Y_COORDINATE_EXIT) {
        nodelay(stdscr, FALSE);
        printInMiddle(gameWindow, MAP_WIDTH, MAP_HEIGHT, GAME_OVER_WON);
        wrefresh(gameWindow);
    }*/

    posX *= SQUARE_WIDTH;

    P(semId, SEM_MAP, 1);
    drawSquare(gameWindow, PLAYER_SQUARE, posX, posY, TRUE);
    updateMoves(stateWindow, map);
    updateLivesLeft(stateWindow, map);
    updateDiscoveredWalls(stateWindow, map);
    V(semId, SEM_MAP, 1);

    nodelay(stdscr, TRUE);
    while (run && (i = getch()) != KEY_F(2)) {
        P(semId, SEM_MAP, 1);

        if (map->livesLeft > 0) {
            if ((posX != X_COORDINATE_ENTRANCE || posY != Y_COORDINATE_ENTRANCE) && isMinotaurNearby(map)) {
                drawSquare(gameWindow, VISITED_SQUARE, posX, posY, FALSE);
                drawSquare(gameWindow, PLAYER_SQUARE, map->posX * SQUARE_WIDTH, map->posY, FALSE);
                posX = map->posX;
                posY = map->posY;

                if(cpt != 0)
                    wprintw(informationWindow, "\n");

                wprintw(informationWindow, "You have lost one life to a minotaur, %d left!", map->livesLeft);
                wrefresh(informationWindow);

                cpt++;

                updateLivesLeft(stateWindow, map);

                if (map->livesLeft == 0) {
                    printInMiddle(gameWindow, MAP_WIDTH, MAP_HEIGHT, GAME_OVER_LOST);
                }

                wrefresh(gameWindow);
            }

            if (i == ERR) {
                drawMap(gameWindow, map);
            }

            else {
                previousPosX = posX;
                previousPosY = posY;

                if (map->livesLeft != 0 && (posX / SQUARE_WIDTH != X_COORDINATE_EXIT || posY != Y_COORDINATE_EXIT)) {
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

                    newSquare = movePlayer(map, (unsigned char) (posX / SQUARE_WIDTH), posY);

                    switch (newSquare) {
                        case PLAYER_SQUARE:
                            /* Player has moved, display the current square as the player's position/color */

                            drawSquare(gameWindow, PLAYER_SQUARE, posX, posY, FALSE);
                            updateMoves(stateWindow, map);

                            break;

                        case DISCOVERED_WALL:
                            /* Player has hit an invisible wall that has become "discovered", draw it as such */

                            drawSquare(gameWindow, DISCOVERED_WALL, posX, posY, FALSE);
                            updateDiscoveredWalls(stateWindow, map);

                            if(cpt != 0) {
                                wprintw(informationWindow, "\n");
                            }

                            wprintw(informationWindow, "You have found an invisible wall!");
                            wrefresh(informationWindow);

                            cpt++;

                            /* Don't need "break" here */

                        case UNCHANGED:
                            /* We either encountered a wall (previous case, followed by this one) or there was an error, so simply set the position back to what it was before the move */
                            posX = previousPosX;
                            posY = previousPosY;

                            drawSquare(gameWindow, PLAYER_SQUARE, posX, posY, FALSE);

                            break;

                        default:
                            break;
                    }
                }
            }
        }

        if ((posX / SQUARE_WIDTH == X_COORDINATE_EXIT && posY == Y_COORDINATE_EXIT) || map->livesLeft == 0) {
            nodelay(stdscr, FALSE);

            if (map->livesLeft == 0) {
                printInMiddle(gameWindow, MAP_WIDTH, MAP_HEIGHT, GAME_OVER_LOST);
            }

            else {
                discoverAllWalls(gameWindow, map);
                printInMiddle(gameWindow, MAP_WIDTH, MAP_HEIGHT, GAME_OVER_WON);
            }

            if (!sent) {
                sent = TRUE;
                request.type = GAME_OVER_TYPE;
                sendMessage(queueId, &request, sizeof(request_t), 0);
            }
        }

        wrefresh(gameWindow);
        V(semId, SEM_MAP, 1);
    }

    request.type = DISCONNECT_TYPE;
    request.message.disconnect.pid = getpid();
    request.message.disconnect.programType = PLAYER;
    sendMessage(queueId, &request, sizeof(request_t), 0);

    V(semId, SEM_PLAYER, 1);

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
