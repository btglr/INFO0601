#include <curses.h> /* Pour printw, attron, attroff, COLOR_PAIR, getch */
#include <stdlib.h>
#include <stdlib.h> /* Pour EXIT_FAILURE */
#include <string.h>

#include "ncurses.h"

#define LARGEUR 20 /* Largeur de la fenêtre */
#define HAUTEUR 10 /* Hauteur de la fenêtre */
#define POSX 20    /* Position horizontale de la fenêtre */
#define POSY 5     /* Position verticale de la fenêtre */

int main() {
    WINDOW* fenetre;

    /* Initialisation de ncurses */
    ncurses_initialiser();
    ncurses_souris();
    ncurses_couleurs();

    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);

    clear();
    refresh();
    /* Création de la fenêtre */
    fenetre = newwin(HAUTEUR, LARGEUR, POSY, POSX);

    /* Suppression de la fenêtre */
    delwin(fenetre);

    /* Arrêt de ncurses */
    ncurses_stopper();

    return EXIT_SUCCESS;
}