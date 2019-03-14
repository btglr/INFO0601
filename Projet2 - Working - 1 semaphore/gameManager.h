/**
 * @file gameManager.h
 * @author Bastien T.
 * @date 21 January 2018
 * @brief Functions pertaining to the game manager
 */

#ifndef TP3_GAMEMANAGER_H
#define TP3_GAMEMANAGER_H

#include <curses.h>
#include <sys/types.h>
#include "structures.h"

/**
 * Loads the @p filename whether it be a save file or a map file
 * @param filename The file to load
 * @return A file descriptor to the file save
 */
int loadGame(char *filename);

/**
 * Moves the player to the given @p newX and @p newY coordinates
 * @param fd The file descriptor of the save file
 * @param newX The new X coordinate of the player
 * @param newY The new Y coordinate of the player
 * @return The new square where the player is now positioned, or UNCHANGED
 */
unsigned char movePlayer(map_t *map, unsigned char newX, unsigned char newY);

/**
 * Removes one life from the player
 * @param map The file descriptor of the save file
 * @return The number of lives remaining
 */
int loseLife(map_t *map);

/**
 * Loads the initial state window for the game executable
 * @param window The window to load it in
 */
void loadStateWindowGame(WINDOW *window);

/**
 * Changes the wall type for the game executable
 * @param map The file descriptor of the save file
 * @param x The x position of the wall
 * @param y The y position of the wall
 * @return The new wall type
 */
unsigned char changeWallGame(map_t *map, int x, int y);

/**
 * Changes all walls of a given type to the next type
 * @param map The file descriptor of the save file
 * @param type The type of the walls we want to change to the next one
 */
void changeAllWalls(map_t *map, unsigned char type);

/**
 * Gets the next wall type for the game executable
 * @param type The current type of the wall
 * @return The next type of the wall
 */
unsigned char getNextWallGame(unsigned char type);

/**
 * Updates the state window with the current amount of discovered walls
 * @param window The window to update
 * @param map The file descriptor of the save file
 */
void updateDiscoveredWalls(WINDOW *window, map_t *map);

/**
 * Updates the state window with the current amount of moves
 * @param window The window to update
 * @param map The file descriptor of the save file
 */
void updateMoves(WINDOW *window, map_t *map);

/**
 * Updates the state window with the current and total amount of lives
 * @param window The window to update
 * @param map The file descriptor of the save file
 */
void updateLivesLeft(WINDOW *window, map_t *map);

/**
 * Discovers all invisible walls (when you win...)
 * @param window The game window to update
 * @param map The file descriptor of the save file
 */
void discoverAllWalls(WINDOW *window, map_t *map);

/**
 * Gets the player position
 * @param map The file descriptor of the save file
 * @param x The variable that will contain the x position
 * @param y The variable that will contain the y position
 */
void getPlayerPosition(map_t *map, unsigned char *x, unsigned char *y);

/**
 * Sets the player position
 * @param map The file descriptor of the save file
 * @param x The new x position
 * @param y The new y position
 */
void setPlayerPosition(map_t *map, unsigned char x, unsigned char y);

/**
 * Gets the amount of remaining lives
 * @param fd The file descriptor of the save file
 * @return The amount of remaining lives
 */
unsigned char getRemainingLives(int fd);

/**
 * Sets the amount of remaining lives
 * @param fd The file descriptor of the save file
 * @param lives The amount of lives to set
 * @return The number of bytes written
 */
ssize_t setRemainingLives(int fd, unsigned char lives);

bool isMinotaurNearby(map_t *map);

#endif
