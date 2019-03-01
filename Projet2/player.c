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

bool run = TRUE;

void handler(int sig) {
    if (sig == SIGINT) {
        run = FALSE;
    }
}

int main(int argc, char *argv[]) {
    int i, queueId, semId, shmId;
    WINDOW *borderInformationWindow, *informationWindow, *borderGameWindow, *gameWindow, *borderStateWindow, *stateWindow;
    request_t request;
    response_t response;
    struct sigaction action;

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

    request.type = GET_KEYS_TYPE;
    queueId = openMessageQueue(KEY_MESSAGE_QUEUE);
    sendMessage(queueId, &request, sizeof(request_t), 0);

    receiveMessage(queueId, &response, sizeof(response_t), RESPONSE_TYPE, 0);

    semId = response.keySem;
    shmId = response.keyShm;

    wprintw(informationWindow, "Sem id: %d, Shm id: %d\n", semId, shmId);

    P(semId, SEM_PLAYER, 1);
    wprintw(informationWindow, "Semaphore value: %d\n", getSemaphoreValue(semId, SEM_PLAYER));
    wrefresh(informationWindow);

    request.type = CONNECT_TYPE;
    request.message.connect.pid = getpid();
    request.message.connect.programType = PLAYER;
    sendMessage(queueId, &request, sizeof(request_t), 0);

    while (run && (i = getch()) != KEY_F(2)) {

    }

    V(semId, SEM_PLAYER, 1);

    request.type = DISCONNECT_TYPE;
    request.message.disconnect.pid = getpid();
    request.message.disconnect.programType = PLAYER;
    sendMessage(queueId, &request, sizeof(request_t), 0);

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
