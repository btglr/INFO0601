#ifndef _NCURSES_
#define _NCURSES_

/**
 * Initialisation de ncurses.
 */
void initialize_ncurses();

/**
 * Fin de ncurses.
 */
void stop_ncurses();

/**
 * Initialisation des couleurs.
 */
void ncurses_colors();

/**
 * Initialisation de la souris.
 */
void ncurses_mouse();

/**
 * Recupere la position x et y de la souris.
 * @param[out] x la position en x
 * @param[out] y la position en y
 * @param[out] button l'évenement associé au clic (ou NULL)
 * @return OK si réussite
 */
int mouse_getpos(int *x, int *y, int *button);

#endif
