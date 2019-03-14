/**
 * @file windowDrawer.h
 * @author Bastien T.
 * @date 21 January 2018
 * @brief Utility functions for the windows
 */

#ifndef TP3_WINDOWDRAWER_H
#define TP3_WINDOWDRAWER_H

#include "structures.h"

/**
 * Initializes and returns a window with the given parameters
 * @param width The width of the window
 * @param height The height of the window
 * @param x The x coordinate of the window
 * @param y The y coordinate of the window
 * @return The newly created window
 */
WINDOW* initializeWindow(int width, int height, int x, int y);

/**
 * Initializes and returns a sub-window of the specified @ window
 * @param window The window to make a sub-window of
 * @param width The width of the sub-window
 * @param height The height of the sub-window
 * @param x The x coordinate of the sub-window
 * @param y The y coordinate of the sub-window
 * @return The newly created subwindow
 */
WINDOW* initializeSubWindow(WINDOW *window, int width, int height, int x, int y);

/**
 * Draws a square into the given window
 * @param window The window to draw into
 * @param type The type of square to draw
 * @param x The x coordinate of the square
 * @param y The y coordinate of the square
 * @param refresh TRUE or FALSE, determines whether to refresh the window after drawing
 */
void drawSquare(WINDOW *window, int type, int x, int y, bool refresh);

/**
 * Draws the map from a file
 * @param window The game window
 * @param fd The file descriptor to the map
 */
void drawMapFile(WINDOW *window, int fd);

/**
 * Draws the map from a map struct
 * @param window The game window
 * @param semId The array of semaphores' id
 * @param fd The map
 */
void drawMap(WINDOW *window, int semId, map_t *map);

/**
 * Prints a message in the middle of the given window
 * @param window The window to draw into
 * @param maxWidth The maximum width of the window
 * @param maxHeight The maximum height of the window
 * @param text The text to print
 */
void printInMiddle(WINDOW *window, int maxWidth, int maxHeight, char *text);

/**
 * Updates the state window with the given parameters
 * @param window The state window
 * @param x The x coordinate to print at
 * @param y The y coordinate to print at
 * @param s The format string
 * @param ... Additional parameters depending on the format, like printf/scanf
 */
void updateStateWindow(WINDOW *window, int x, int y, char *s, ...);

/**
 * Clears the line starting at coordinate @p x and @p y
 * @param window The window to clear the line from
 * @param x The x coordinate to clear at
 * @param y The y coordinate to clear at
 */
void clearLine(WINDOW *window, int x, int y);

#endif
