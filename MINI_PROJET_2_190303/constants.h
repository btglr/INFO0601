/**
 * @file constants.h
 * @author Bastien T.
 * @date 21 January 2018
 * @brief File containing all the constants
 */

#ifndef TP3_CONSTANTS_H
#define TP3_CONSTANTS_H

/**
 * The X coordinate of the entrance
 */
#define X_COORDINATE_ENTRANCE 0

/**
 * The Y coordinate of the entrance
 */
#define Y_COORDINATE_ENTRANCE 8

/**
 * The X coordinate of the exit
 */
#define X_COORDINATE_EXIT 29

/**
 * The Y coordinate of the exit
 */
#define Y_COORDINATE_EXIT 8

/**
 * The default number for the map version
 */
#define DEFAULT_MAP_VERSION 0

/**
 * The default amount of lives when initializing a new map
 */
#define DEFAULT_LIVES 10

/**
 * The map width
 */
#define MAP_WIDTH 30

/**
 * The map height
 */
#define MAP_HEIGHT 15

/**
 * Constant used when there was an error, means whatever was asked was left unchanged
 */
#define UNCHANGED 255

/**
 * The value corresponding to an empty square in the map file
 */
#define EMPTY_SQUARE 0

/**
 * The value corresponding to an invisible wall in the map file
 */
#define INVISIBLE_WALL 1

/**
 * The value corresponding to a visible wall in the map file
 */
#define VISIBLE_WALL 2

/**
 * The value corresponding to a discovered wall in the map file
 */
#define DISCOVERED_WALL 3

/**
 * The value corresponding to a visited square in the map file
 */
#define VISITED_SQUARE 4

/**
 * The value corresponding to the player (NOT saved in the map file, but present in the shared memory segment)
 */
#define PLAYER_SQUARE 10

/**
 * Determines the width (in characters) of the square in the graphical interface
 */
#define SQUARE_WIDTH 2

/**
 * The color corresponding to an empty square
 */
#define COLOR_EMPTY_SQUARE COLOR_BLACK

/**
 * The color corresponding to a visible wall
 */
#define COLOR_VISIBLE_WALL COLOR_BLUE

/**
 * The color corresponding to an invisible wall
 */
#define COLOR_INVISIBLE_WALL COLOR_MAGENTA

/**
 * The color corresponding to a discovered wall
 */
#define COLOR_DISCOVERED_WALL COLOR_RED

/**
 * The color corresponding to the player
 */
#define COLOR_PLAYER COLOR_GREEN

/**
 * The color corresponding to a visited square
 */
#define COLOR_VISITED_SQUARE COLOR_WHITE

/**
 * The color corresponding to a minotaur
 */
#define COLOR_MINOTAUR COLOR_YELLOW

/**
 * The pair number of the player's color
 */
#define PAIR_COLOR_PLAYER 3

/**
 * The pair number of the visible walls' color
 */
#define PAIR_COLOR_VISIBLE_WALL 4

/**
 * The pair number of the invisible walls' color
 */
#define PAIR_COLOR_INVISIBLE_WALL 5

/**
 * The pair number of the empty squares' color
 */
#define PAIR_COLOR_EMPTY_SQUARE 6

/**
 * The pair number of the plus sign's color
 */
#define PAIR_COLOR_PLUS_SIGN 7

/**
 * The pair number of the minus sign's color
 */
#define PAIR_COLOR_MINUS_SIGN 8

/**
 * The pair number of the discovered walls' color
 */
#define PAIR_COLOR_DISCOVERED_WALL 9

/**
 * The pair number of the visited squares' color
 */
#define PAIR_COLOR_VISITED_SQUARE 10

/**
 * The pair number of the minotaurs
 */
#define PAIR_COLOR_MINOTAUR 11

/**
 * The border width to be used to calculate the size of the windows
 * Does NOT define the width of the border
 */
#define BORDER_WIDTH 2

/**
 * The border height to be used to calculate the size of the windows
 * Does NOT define the height of the border
 */
#define BORDER_HEIGHT 2

/**
 * The width (border included) of the state window (depends of the SQUARE_WIDTH)
 */
#define BORDER_STATE_WINDOW_WIDTH 20 * SQUARE_WIDTH + BORDER_WIDTH

/**
 * The height (border included) of the state window
 */
#define BORDER_STATE_WINDOW_HEIGHT 15 + BORDER_HEIGHT

/**
 * The width (border included) of the information window (depends of the SQUARE_WIDTH)
 */
#define BORDER_INFORMATION_WINDOW_WIDTH MAP_WIDTH * SQUARE_WIDTH + BORDER_STATE_WINDOW_WIDTH + BORDER_WIDTH

/**
 * The height (border included) of the information window
 */
#define BORDER_INFORMATION_WINDOW_HEIGHT 4 + BORDER_HEIGHT

/**
 * The width (border included) of the game window (depends of the SQUARE_WIDTH)
 */
#define BORDER_GAME_WINDOW_WIDTH MAP_WIDTH * SQUARE_WIDTH + BORDER_WIDTH

/**
 * The height (border included) of the game window
 */
#define BORDER_GAME_WINDOW_HEIGHT MAP_HEIGHT + BORDER_HEIGHT

/**
 * The X coordinate of the plus sign, relative to the state window
 */
#define X_COORDINATE_PLUS_SIGN 12

/**
 * The Y coordinate of the plus sign, relative to the state window
 */
#define Y_COORDINATE_PLUS_SIGN 1

/**
 * The X coordinate of the minus sign, relative to the state window
 */
#define X_COORDINATE_MINUS_SIGN 18

/**
 * The Y coordinate of the minus sign, relative to the state window
 */
#define Y_COORDINATE_MINUS_SIGN 1

/**
 * The maximum length for filenames (map or save)
 */
#define MAX_FILENAME_LENGTH 256

/**
 * The message displayed when losing
 */
#define GAME_OVER_LOST "Game Over. The minotaurs have defeated you!"

/**
 * The message displayed when winning
 */
#define GAME_OVER_WON "Game Over. You have defeated the minotaurs!"

/**
 * The folder where maps are saved and opened from
 */
#define MAPS_FOLDER "maps"

/**
 * The folder where saves are saved and opened from
 */
#define SAVES_FOLDER "saves"

/**
 * Maximum number of minotaurs
 */
#define MAX_MINOTAUR 5

/**
 * The (long) type of a struct to send through the message queue
 */
#define GET_KEYS_TYPE 1

/**
 * The (long) type of a struct to send through the message queue
 */
#define CONNECT_TYPE 2

/**
 * The (long) type of a struct to send through the message queue
 */
#define DISCONNECT_TYPE 3

/**
 * The (long) type of a struct to send through the message queue
 */
#define GAME_OVER_TYPE 4

/**
 * The (long) type of a struct to send through the message queue
 */
#define RESPONSE_TYPE 20

/**
 * The value of the program type set in the struct to send through the message queue
 */
#define MINOTAUR 10

/**
 * The value of the program type set in the struct to send through the message queue
 */
#define PLAYER 20

/**
 * How long the minotaurs should sleep
 */
#define SLEEP_MINOTAUR 1

/**
 * The id of the minotaur semaphore
 */
#define SEM_MINOTAUR 0

/**
 * The id of the player semaphore
 */
#define SEM_PLAYER 1

/**
 * The id of the first chunk's semaphore
 */
#define SEM_MAP_CHUNK 2

/**
 * The first minotaur's id
 */
#define MINOTAUR_ID_BEGIN 11

/**
 * The last minotaur's id
 */
#define MINOTAUR_ID_END MINOTAUR_ID_BEGIN + MAX_MINOTAUR

/**
 * The character to use to display a minotaur
 */
#define MINOTAUR_CHAR '|'

/**
 * The size of a chunk
 */
#define CHUNK_SIZE 5

#define UP 0
#define DOWN 1
#define RIGHT 2
#define LEFT 3
#define LEFT_UP 4
#define LEFT_DOWN 5
#define RIGHT_UP 6
#define RIGHT_DOWN 7

#endif