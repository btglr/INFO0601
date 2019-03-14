/**
 * @file mapUtils.h
 * @author Bastien T.
 * @date 21 January 2018
 * @brief Utility functions for both the game manager and the map editor
 */

#ifndef TP3_MAPUTILS_H
#define TP3_MAPUTILS_H

#include "structures.h"

/**
 * Gets the total number of lives in the file
 * @param fd The file descriptor of the save file
 * @return The number of lives
 */
unsigned char getTotalLives(int fd);

/**
 * Gets the map version from the file
 * @param fd The file descriptor of the save file
 * @return The map version
 */
int getMapVersion(int fd);

/**
 * Increases the total number of lives
 * @param fd The file descriptor of the save file
 */
void increaseTotalLives(int fd);

/**
 * Decreases the total number of lives
 * @param fd The file descriptor of the save file
 */
void decreaseTotalLives(int fd);

/**
 * Gets the amount of walls of a given type
 * @param fd The file descriptor of the save file
 * @param type The type of wall to count
 * @return The amount of walls of the type
 */
int getWallCountFile(int fd, int type);

/**
 * Gets the amount of walls of a given type
 * @param map The pointer to the map
 * @param type The type of wall to count
 * @return The amount of walls of the type
 */
int getWallCount(map_t *map, int type);

/**
 * Increases the map version of the file
 * @param fd The file descriptor of the save file
 */
void increaseMapVersion(int fd);

/**
 * Sets the wall at the given position to the given type
 * @param fd The file descriptor of the save file
 * @param x The x position of the wall
 * @param y The y position of the wall
 * @param type The new type of the wall
 * @return
 */
unsigned char setWallAt(int fd, int x, int y, unsigned char type);

/**
 * Gets the next wall at a given position depending on whether it's the game or editor
 * @param fd The file descriptor of the save file
 * @param x The x position of the wall
 * @param y The y position of the wall
 * @param editor TRUE or FALSE
 * @return The next wall at the given position
 */
unsigned char getNextWallAt(int fd, int x, int y, int editor);

/**
 * Makes a value v a multiple of m
 * @param v The value
 * @param m The multiple
 * @return The new value
 */
int makeMultipleOf(int v, int m);

#endif
