/**
 * @file ncurses.h
 * @author Cyril R.
 * @brief Utility functions for ncurses
 */

#ifndef _NCURSES_
#define _NCURSES_

/**
 * Initializes ncurses
 */
void initialize_ncurses();

/**
 * Stops ncurses
 */
void stop_ncurses();

/**
 * Initializes ncurses colors
 */
void ncurses_colors();

/**
 * Initializes the mouse
 */
void ncurses_mouse();

/**
 * Gets the mouse's X and Y position
 * @param[out] x the x position
 * @param[out] y the y position
 * @param[out] button the event associated to the click (or NULL)
 * @return
 */
int mouse_getpos(int *x, int *y, int *button);

#endif
