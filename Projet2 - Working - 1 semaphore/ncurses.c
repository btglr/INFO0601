#include "ncurses.h"

#include <curses.h> /* Pour toutes les fonctions/constantes ncurses */
#include <stdlib.h> /* Pour exit, EXIT_FAILURE */

void initialize_ncurses() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    refresh();
    curs_set(FALSE);
}

void stop_ncurses() {
    endwin();
}

void ncurses_colors() {
    /* Checking if color is supported */
    if (has_colors() == FALSE) {
        stop_ncurses();
        fprintf(stderr, "The terminal can't handle colors.\n");
        exit(EXIT_FAILURE);
    }

    /* Activating the colors */
    start_color();

    /* Creating the color pairs */
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_RED);
}

void ncurses_mouse() {
    if (!mousemask(ALL_MOUSE_EVENTS, NULL)) {
        stop_ncurses();
        fprintf(stderr, "An error occurred while initializing the mouse.\n");
        exit(EXIT_FAILURE);
    }

    if (has_mouse() != TRUE) {
        stop_ncurses();
        fprintf(stderr, "Error: No mouse detected.\n");
        exit(EXIT_FAILURE);
    }
}

int mouse_getpos(int *x, int *y, int *button) {
    MEVENT event;
    int result = getmouse(&event);

    if (result == OK) {
        *x = event.x;
        *y = event.y;
        if (button != NULL)
            *button = event.bstate;
    }
    return result;
}