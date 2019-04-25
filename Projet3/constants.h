#ifndef PROJET3_CONSTANTS_H
#define PROJET3_CONSTANTS_H

#define MAP_WIDTH 30
#define MAP_HEIGHT 15
#define NUMBER_LEMMINGS 5

#define MAX_FILENAME_LENGTH 255

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

#define QUEUE_SIZE 2

#endif
