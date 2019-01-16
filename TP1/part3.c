#include <curses.h> /* Pour printw, attron, attroff, COLOR_PAIR, getch */
#include <stdlib.h>
#include <stdlib.h> /* Pour EXIT_FAILURE */
#include <string.h>

#include "ncurses.h"

#define HAUTEUR 10
#define LARGEUR 20
#define POSY 10

void drawGameWindow(WINDOW *borderInnerGameWindow) {
    mvwprintw(borderInnerGameWindow, 0, 2, "Jeu");
    wrefresh(borderInnerGameWindow);
}

void drawScoreboard(WINDOW *borderInnerScoreWindow, int color1, int color2) {
    mvwprintw(borderInnerScoreWindow, 0, 2, "Score");

    wattron(borderInnerScoreWindow, color1);
    mvwprintw(borderInnerScoreWindow, 2, 1, "  ");
    wattroff(borderInnerScoreWindow, color1);

    mvwprintw(borderInnerScoreWindow, 2, 4, "Joueur 1");

    wattron(borderInnerScoreWindow, color2);
    mvwprintw(borderInnerScoreWindow, 4, 1, "  ");
    wattroff(borderInnerScoreWindow, color2);

    mvwprintw(borderInnerScoreWindow, 4, 4, "Joueur 2");
    wrefresh(borderInnerScoreWindow);
}

void drawSquare(WINDOW *borderInnerGameWindow, int posY, int posX, int color) {
    wattron(borderInnerGameWindow, color);
    mvwprintw(borderInnerGameWindow, posY, posX, "  ");
    wattroff(borderInnerGameWindow, color);
}

/* void drawLine(WINDOW *borderInnerGameWindow, int line) {
    drawSquare(borderInnerGameWindow, line, 1, COLOR_PAIR(7));
    drawSquare(borderInnerGameWindow, line, 3, COLOR_PAIR(8));
    drawSquare(borderInnerGameWindow, line, 5, COLOR_PAIR(7));
    drawSquare(borderInnerGameWindow, line, 7, COLOR_PAIR(8));
    drawSquare(borderInnerGameWindow, line, 9, COLOR_PAIR(7));
    drawSquare(borderInnerGameWindow, line, 11, COLOR_PAIR(8));
    drawSquare(borderInnerGameWindow, line, 13, COLOR_PAIR(7));
    drawSquare(borderInnerGameWindow, line, 15, COLOR_PAIR(8));
    drawSquare(borderInnerGameWindow, line, 17, COLOR_PAIR(7));
    drawSquare(borderInnerGameWindow, line, 19, COLOR_PAIR(8));
    wrefresh(borderInnerGameWindow);
}

void drawColumn(WINDOW *borderInnerGameWindow, int column) {
    drawSquare(borderInnerGameWindow, 1, column, COLOR_PAIR(7));
    drawSquare(borderInnerGameWindow, 2, column, COLOR_PAIR(8));
    drawSquare(borderInnerGameWindow, 3, column, COLOR_PAIR(7));
    drawSquare(borderInnerGameWindow, 4, column, COLOR_PAIR(8));
    drawSquare(borderInnerGameWindow, 5, column, COLOR_PAIR(7));
    drawSquare(borderInnerGameWindow, 6, column, COLOR_PAIR(8));
    drawSquare(borderInnerGameWindow, 7, column, COLOR_PAIR(7));
    drawSquare(borderInnerGameWindow, 8, column, COLOR_PAIR(8));
    drawSquare(borderInnerGameWindow, 9, column, COLOR_PAIR(7));
    drawSquare(borderInnerGameWindow, 10, column, COLOR_PAIR(8));
    wrefresh(borderInnerGameWindow);
} */

void updateScoreboard(WINDOW *borderInnerScoreWindow, int player, int points) {
    switch(player) {
        case 1:
        case 2:
            mvwprintw(borderInnerScoreWindow, player * 2, 4, "Joueur %d: %d pts", player, points);
            break;
    }

    wrefresh(borderInnerScoreWindow);
}

int main() {
    int i;
    WINDOW *borderGameWindow, *borderInnerGameWindow, *borderScoreWindow, *borderInnerScoreWindow;

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
    /* Creating the windows */
    borderGameWindow = newwin(HAUTEUR + 4, LARGEUR + 4, 0, 0);
    borderInnerGameWindow = subwin(borderGameWindow, HAUTEUR + 2, LARGEUR + 2, 1, 1);
    box(borderGameWindow, 0, 0);
    box(borderInnerGameWindow, 0, 0);

    borderScoreWindow = newwin(8, LARGEUR + 4, HAUTEUR + 4, 0);
    borderInnerScoreWindow = subwin(borderScoreWindow, 6, LARGEUR + 2, HAUTEUR + 5, 1);
    box(borderScoreWindow, 0, 0);
    box(borderInnerScoreWindow, 0, 0);

    wrefresh(borderGameWindow);
    wrefresh(borderInnerGameWindow);
    wrefresh(borderScoreWindow);
    wrefresh(borderInnerScoreWindow);

    drawGameWindow(borderInnerGameWindow);
    drawScoreboard(borderInnerScoreWindow, COLOR_PAIR(7), COLOR_PAIR(8));

    updateScoreboard(borderInnerScoreWindow, 1, 2);
    updateScoreboard(borderInnerScoreWindow, 2, 5);

    while ((i = getch()) != KEY_F(2)) {

    }

    /* Delete the windows */
    delwin(borderInnerScoreWindow);
    delwin(borderScoreWindow);
    delwin(borderInnerGameWindow);
    delwin(borderGameWindow);

    /* Stopping ncurses */
    ncurses_stopper();

    return EXIT_SUCCESS;
}