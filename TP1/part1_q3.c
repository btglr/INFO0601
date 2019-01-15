#include <curses.h> /* Pour printw, attron, attroff, COLOR_PAIR, getch */
#include <stdlib.h>
#include <stdlib.h> /* Pour EXIT_FAILURE */
#include <string.h>

#include "ncurses.h"

#define LARGEUR 50 /* Largeur de la fenêtre */
#define HAUTEUR 25 /* Hauteur de la fenêtre */
#define POSX 20    /* Position horizontale de la fenêtre */
#define POSY 5     /* Position verticale de la fenêtre */

int main() {
    int i, couleur;
    int sourisX, sourisY;
    char* texte = "Bonjour";
    int hauteurCarre = 2, largeurCarre = 4;
    int fondJaune, fondRouge, fondVert;
    int texteJaune, texteRouge, texteVert;
    WINDOW* fenetre;

    /* Initialisation de ncurses */
    ncurses_initialiser();
    ncurses_souris();
    ncurses_couleurs();

    /* Vérification des dimensions du terminal */
    if ((COLS < POSX + LARGEUR) || (LINES < POSY + HAUTEUR)) {
        ncurses_stopper();
        fprintf(stderr,
                "Les dimensions du terminal sont insufisantes : l=%d,h=%d au lieu de l=%d,h=%d\n",
                COLS, LINES, POSX + LARGEUR, POSY + HAUTEUR);
        exit(EXIT_FAILURE);
    }

    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_BLACK, COLOR_YELLOW);
    init_pair(7, COLOR_BLACK, COLOR_RED);
    init_pair(8, COLOR_BLACK, COLOR_GREEN);

    texteJaune = COLOR_PAIR(3);
    texteRouge = COLOR_PAIR(4);
    texteVert = COLOR_PAIR(5);
    fondJaune = COLOR_PAIR(6);
    fondRouge = COLOR_PAIR(7);
    fondVert = COLOR_PAIR(8);

    /* Création de la fenêtre */
    clear();
    refresh();
    fenetre = newwin(HAUTEUR, LARGEUR, POSY, POSX);
    box(fenetre, 0, 0);

    mvwprintw(fenetre, 5, (int)((LARGEUR / 2) - strlen(texte) / 2), texte);
    wrefresh(fenetre);

    wattron(fenetre, fondJaune);
    mvwprintw(fenetre, 15, ((LARGEUR / 9) * 2) - (largeurCarre / 2), "    ");
    mvwprintw(fenetre, 16, ((LARGEUR / 9) * 2) - (largeurCarre / 2), "    ");
    wattroff(fenetre, fondJaune);

    wattron(fenetre, fondRouge);
    mvwprintw(fenetre, 15, ((LARGEUR / 9) * 5) - (largeurCarre / 2), "    ");
    mvwprintw(fenetre, 16, ((LARGEUR / 9) * 5) - (largeurCarre / 2), "    ");
    wattroff(fenetre, fondRouge);

    wattron(fenetre, fondVert);
    mvwprintw(fenetre, 15, ((LARGEUR / 9) * 8) - (largeurCarre / 2), "    ");
    mvwprintw(fenetre, 16, ((LARGEUR / 9) * 8) - (largeurCarre / 2), "    ");
    wattroff(fenetre, fondVert);

    wrefresh(fenetre);

    while ((i = getch()) != KEY_F(2)) {
        if ((i == KEY_MOUSE) && (souris_getpos(&sourisX, &sourisY, NULL) == OK)) {
            if (sourisY >= POSY + 15 && sourisY < POSY + 15 + hauteurCarre) {
                if (sourisX >= (POSX + ((LARGEUR / 9) * 2) - (largeurCarre / 2)) && sourisX < (POSX + ((LARGEUR / 9) * 2) + (largeurCarre / 2))) {
                    /* Clic sur le premier carré */
                    couleur = texteJaune;
                }

                else if (sourisX >= (POSX + ((LARGEUR / 9) * 5) - (largeurCarre / 2)) && sourisX < (POSX + ((LARGEUR / 9) * 5) + (largeurCarre / 2))) {
                    /* Clic sur le second carré */
                    couleur = texteRouge;
                }

                else if (sourisX >= (POSX + ((LARGEUR / 9) * 8) - (largeurCarre / 2)) && sourisX < (POSX + ((LARGEUR / 9) * 8) + (largeurCarre / 2))) {
                    /* Clic sur troisième carré */
                    couleur = texteVert;
                }

                else {
                    couleur = -1;
                }
            }

            else {
                couleur = -1;
            }
        }

        else {
            switch (i) {
                case '1':
                    couleur = texteJaune;
                    break;
                case '2':
                    couleur = texteRouge;
                    break;
                case '3':
                    couleur = texteVert;
                    break;
                default:
                    couleur = -1;
            }
        }

        if (couleur != -1) {
            wattron(fenetre, couleur);
            mvwprintw(fenetre, 5, (int)((LARGEUR / 2) - strlen(texte) / 2), texte);
            wattroff(fenetre, couleur);

            wrefresh(fenetre);
        }
    }

    /* Suppression de la fenêtre */
    delwin(fenetre);

    /* Arrêt de ncurses */
    ncurses_stopper();

    return EXIT_SUCCESS;
}