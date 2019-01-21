#include "ncurses.h"

#include <curses.h> /* Pour toutes les fonctions/constantes ncurses */
#include <stdlib.h> /* Pour exit, EXIT_FAILURE */

/**
 * Initialisation de ncurses.
 */
void initialize_ncurses() {
    initscr();            /* Demarre le mode ncurses */
    cbreak();             /* Pour les saisies clavier (desac. mise en buffer) */
    noecho();             /* Desactive l'affichage des caracteres saisis */
    keypad(stdscr, TRUE); /* Active les touches specifiques */
    refresh();            /* Met a jour l'affichage */
    curs_set(FALSE);      /* Masque le curseur */
}

/**
 * Fin de ncurses.
 */
void stop_ncurses() {
    endwin();
}

/**
 * Initialisation des couleurs.
 */
void ncurses_colors() {
    /* Verification du support de la couleur */
    if (has_colors() == FALSE) {
        stop_ncurses();
        fprintf(stderr, "Le terminal ne supporte pas les couleurs.\n");
        exit(EXIT_FAILURE);
    }

    /* Activation des couleurs */
    start_color();

    /* Definition de la palette */
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_BLACK, COLOR_RED);
}

/**
 * Initialisation de la souris.
 */
void ncurses_mouse() {
    if (!mousemask(ALL_MOUSE_EVENTS, NULL)) {
        stop_ncurses();
        fprintf(stderr, "Erreur lors de l'initialisation de la souris.\n");
        exit(EXIT_FAILURE);
    }

    if (has_mouse() != TRUE) {
        stop_ncurses();
        fprintf(stderr, "Aucune souris n'est détectée.\n");
        exit(EXIT_FAILURE);
    }
}

/**
 * Recupere la position x et y de la souris.
 * @param[out] x la position en x
 * @param[out] y la position en y
 * @param[out] button l'évenement associé au clic (ou NULL)
 * @return OK si reussite
 */
int mouse_getpos(int *x, int *y, int *button) {
    MEVENT event;
    int resultat = getmouse(&event);

    if (resultat == OK) {
        *x = event.x;
        *y = event.y;
        if (button != NULL)
            *button = event.bstate;
    }
    return resultat;
}