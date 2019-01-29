/**
 * @file mapEditor.h
 * @author Bastien T.
 * @date 21 January 2018
 * @brief Utility functions for the map editor
 */

#ifndef __MAPEDITOR_H__
#define __MAPEDITOR_H__

#include <curses.h>

/**
 * Initializes a new map with a default number of lives, a map version and all the squares as 0
 * @param fd A file descriptor to the new map
 */
void initializeMap(int fd);

/**
 * Either loads or creates a new map from given filename
 * @param mapName The map's name
 * @return A file descriptor to the map
 */
int loadMapEditor(char *mapName);

/**
 * Changes the wall at coordinates @p x and @p y to the next wall type
 * @param fd The file descriptor to the map file
 * @param x The x coordinate of the wall
 * @param y The y coordinate of the wall
 * @return The new wall at the given coordinates
 */
unsigned char changeWallEditor(int fd, int x, int y);

/**
 * Loads the state window with parameters pertaining to the map editor
 * @param window The state window
 * @param fd The file descriptor to the map file
 */
void loadStateWindowEditor(WINDOW *window, int fd);

/**
 * Updates the wall count in the state window
 * @param window The state window
 * @param fd The file descriptor to the map file
 */
void updateWallCount(WINDOW *window, int fd);

/**
 * Updates the lives count in the state window
 * @param window The state window
 * @param fd The file descriptor to the map file
 */
void updateLivesCount(WINDOW *window, int fd);

/**
 * Given a wall type, returns the next predetermined type
 * @param type The current wall type
 * @return The next wall type
 */
unsigned char getNextWallEditor(unsigned char type);

#endif