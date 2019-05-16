#ifndef PROJET3_CONSTANTS_H
#define PROJET3_CONSTANTS_H

#define NUMBER_LEMMINGS 5

#define SQUARE_WIDTH 2

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
 * The height (border included) of the information window
 */
#define BORDER_INFORMATION_WINDOW_HEIGHT 4 + BORDER_HEIGHT

#define PAIR_COLOR_OBSTACLE 3
#define PAIR_COLOR_EMPTY 4
#define PAIR_COLOR_LEMMING 5
#define PAIR_COLOR_LEGEND 6

/**
 * The color corresponding to an empty square
 */
#define COLOR_EMPTY COLOR_BLACK

/**
 * The color corresponding to an obstacle
 */
#define COLOR_OBSTACLE COLOR_BLUE

#define COLOR_LEMMING COLOR_RED

#define EMPTY 0
#define OBSTACLE 1
#define LEMMING 2

#define NO_TOOL 0
#define TOOL_ADD 1
#define TOOL_REMOVE 2
#define TOOL_EXPLODE 3
#define TOOL_FREEZE 4
#define TOOL_PAUSE_RESUME 5

#define COMMAND_TOOLS_POS_X 2
#define COMMAND_TOOLS_POS_Y 1

#define LEGEND_ADD_LEMMING "+ Add Lemmings"
#define COMMAND_TOOLS_ADD_POS_Y COMMAND_TOOLS_POS_Y + 2
#define COMMAND_TOOLS_REMOVE_POS_Y COMMAND_TOOLS_ADD_POS_Y + 1
#define COMMAND_TOOLS_EXPLODE_POS_Y COMMAND_TOOLS_REMOVE_POS_Y + 1
#define COMMAND_TOOLS_FREEZE_POS_Y COMMAND_TOOLS_EXPLODE_POS_Y + 1
#define COMMAND_TOOLS_PAUSE_RESUME_POS_Y COMMAND_TOOLS_FREEZE_POS_Y + 1

#define LEGEND_POS_X 2
#define LEGEND_POS_Y COMMAND_TOOLS_PAUSE_RESUME_POS_Y + 2

#define LEMMING_CHAR '|'

#define MAX_PATH_LENGTH 512
#define CHUNK_SIZE 512

#define MAPS_FOLDER "maps"

/*
 * Types for communication
 */

    /*
     * Client -> Server
     */
    #define TYPE_CONNECT_UDP_MASTER 1
    #define TYPE_CONNECT_UDP_SLAVE 2

    /*
     * Client -> Client
     */
    #define TYPE_SEND_MAP 3
    #define TYPE_RESPONSE_MAP 4
    #define TYPE_GAME_STATE 5
    #define TYPE_PLAYER_DEATH 6
    #define TYPE_PAUSE_START 7
    #define TYPE_PAUSE_END 8

    /*
     * User commands
     */

    #define TYPE_COMMAND_ADD_LEMMING 9
    #define TYPE_COMMAND_REMOVE_LEMMING 10
    #define TYPE_COMMAND_EXPLODE_LEMMING 11
    #define TYPE_COMMAND_BLOCK_LEMMING 12

    #define TYPE_PAUSE_START_REQUEST 13
    #define TYPE_PAUSE_STOP_REQUEST 14

/*
 * Types defining the state of lemmings
 */

#define TYPE_STATE_REMOVED 0
#define TYPE_STATE_PLACED 1
#define TYPE_STATE_DEAD 2
#define TYPE_STATE_BLOCKED 3

#define QUEUE_SIZE 10

/*
 * MAP GENERATOR
 */

#define MIN_WIDTH 20
#define MIN_HEIGHT 10
#define MIN_DIFFICULTY 1

#define MAX_WIDTH 100
#define MAX_HEIGHT 40
#define MAX_DIFFICULTY 15

#endif
