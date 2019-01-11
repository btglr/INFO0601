#include <stdlib.h>
#include <stdlib.h>     /* Pour EXIT_FAILURE */
#include <ncurses.h>    /* Pour printw, attron, attroff, COLOR_PAIR, getch */
#include <string.h>

#include "ncurses.h"

#define LARGEUR 50      /* Largeur de la fenêtre */
#define HAUTEUR 25      /* Hauteur de la fenêtre */
#define POSX    20      /* Position horizontale de la fenêtre */
#define POSY    5       /* Position verticale de la fenêtre */

int main() {
    int i, color;
    char* texte = "Bonjour";
    WINDOW* fenetre;
    /* int sourisX, sourisY, cpt = 0; */

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

    mvwprintw(fenetre, 5, (LARGEUR/2) - strlen(texte)/2, "Bonjour");
    wrefresh(fenetre);

    while((i = getch()) != KEY_F(2)) {
        switch(i) {
            case 49:
                color = COLOR_PAIR(3);
                break;
            case 50:
                color = COLOR_PAIR(4);
                break;
            case 51:
                color = COLOR_PAIR(5);
                break;
            default:
                color = COLOR_PAIR(1);
        }
        
        wattron(fenetre, color);
        mvwprintw(fenetre, HAUTEUR/2, (LARGEUR/2) - strlen(texte)/2, "Bonjour");
        wattroff(fenetre, color);
        wrefresh(fenetre);
    }


    /* Suppression de la fenêtre */
    delwin(fenetre);

    /* Arrêt de ncurses */
    ncurses_stopper();

    return EXIT_SUCCESS;
}