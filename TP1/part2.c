#include <curses.h> /* Pour printw, attron, attroff, COLOR_PAIR, getch */
#include <stdlib.h>
#include <stdlib.h> /* Pour EXIT_FAILURE */
#include <string.h>

#include "ncurses.h"

#define HAUTEUR 10

int main() {
    int i, mouseX, mouseY, cpt = 0;
    WINDOW *borderInformationWindow, *borderClickWindow, *informationWindow, *clickWindow;

    /* Initialisation de ncurses */
    ncurses_initialiser();
    ncurses_souris();
    ncurses_couleurs();

    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_BLACK, COLOR_YELLOW);
    init_pair(7, COLOR_BLACK, COLOR_RED);
    init_pair(8, COLOR_BLACK, COLOR_GREEN);

    clear();
    refresh();
    /* Création de la fenêtre */
    borderInformationWindow = newwin(HAUTEUR + 2, COLS, 0, 0);
    box(borderInformationWindow, 0, 0);
    borderClickWindow = newwin(HAUTEUR + 2, HAUTEUR + 2, HAUTEUR + 2, 0);
    box(borderClickWindow, 0, 0);

    informationWindow = subwin(borderInformationWindow, HAUTEUR, COLS - 2, 1, 1);
    scrollok(informationWindow, TRUE);

    clickWindow = subwin(borderClickWindow, HAUTEUR, HAUTEUR, HAUTEUR + 3, 1);

    wrefresh(borderInformationWindow);
    wrefresh(borderClickWindow);
    wrefresh(informationWindow);
    wrefresh(clickWindow);

    while ((i = getch()) != KEY_F(2)) {
        if ((i == KEY_MOUSE) && (souris_getpos(&mouseX, &mouseY, NULL) == OK)) {
            if(wmouse_trafo(clickWindow, &mouseY, &mouseX, FALSE) != FALSE) {
                /* If not false, the click was in the window and the new coordinates are in the mouseY and mouseX */
                if(cpt != 0)
                    wprintw(informationWindow, "\n");

                wprintw(informationWindow, "(%d, %d)", mouseX, mouseY);
                wrefresh(informationWindow);

                mvwaddch(clickWindow, mouseY, mouseX, 'X');
                wrefresh(clickWindow);

                cpt++;
            }
        }
    }

    /* Delete the windows */
    delwin(clickWindow);
    delwin(informationWindow);
    delwin(borderClickWindow);
    delwin(borderInformationWindow);

    /* Stopping ncurses */
    ncurses_stopper();

    return EXIT_SUCCESS;
}