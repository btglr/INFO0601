#ifndef PROJET3_STRUCTURES_H
#define PROJET3_STRUCTURES_H

#include "constants.h"
#include <unistd.h>

typedef struct {
    /* TYPE 1 */
    unsigned char type;
    /*
     * Client waits for a connection from a SLAVE client on this (TCP) port
     */
    unsigned short port;
} connect_udp_master_t;

typedef struct {
    /* TYPE 2 */
    unsigned char type;
    /*
     * Client waits for an UDP message with the address of a MASTER client
     */
} connect_udp_slave_t;

typedef struct {
    char *request;
    struct sockaddr *sourceAddr;
    socklen_t addrLen;
} queue_element_t;

typedef struct {
    char address[16];
    unsigned short port;
} send_address_udp_t;

typedef struct {
    /* TYPE 3 */
    unsigned char type;
    unsigned char width, height;
    unsigned char cells[MAP_WIDTH * MAP_HEIGHT];
    /*
     * width and height depend on a constant?
     */
} send_map_t;

typedef struct {
    /* TYPE 4 */
    unsigned char type;
    /*
     * MASTER client waits for this response ("OK") from the SLAVE client
     * Game starts after this
     */
} response_map_t;

/**
 * After game starts
 */

typedef struct {
    /* NOT the same type as requests
     * 0 - removed
     * 1 - in the game
     * 2 - dead
     * 3 - blocked
     */
    unsigned char type;
    unsigned char x, y;
} state_t;

typedef struct {
    /* TYPE 5 */
    unsigned char type;
    state_t states[NUMBER_LEMMINGS * 2];
} game_state_t;

typedef struct {
    /* TYPE 6 */
    unsigned char type;
    /* 1 or 2 */
    unsigned char player;

    /* By MASTER client to SLAVE client */
} player_death_t;

typedef struct {
    /* TYPE 7 */
    unsigned char type;
    /* 1 or 2 */
    unsigned char player;

    /* By MASTER client to SLAVE client */
} pause_start_t;

typedef struct {
    /* TYPE 8 */
    unsigned char type;
} pause_end_t;

/*
 * User commands
 */

typedef struct {
    /* TYPE 9 */
    unsigned char type;
    /* 0 to 4 */
    unsigned char lemmingId;
    /* Positions to add the lemming at */
    unsigned char x, y;
} add_lemming_t;

typedef struct {
    /* TYPE 10 */
    unsigned char type;
    /* 0 to 4 */
    unsigned char lemmingId;
} remove_lemming_t;

typedef struct {
    /* TYPE 11 */
    unsigned char type;
    /* 0 to 4 */
    unsigned char lemmingId;
} explode_lemming_t;

typedef struct {
    /* TYPE 12 */
    unsigned char type;
    /* 0 to 4 */
    unsigned char lemmingId;
} block_lemming_t;

typedef struct {
    /* TYPE 13 */
    unsigned char type;
} pause_start_request_t;

typedef struct {
    /* TYPE 14 */
    unsigned char type;
} pause_stop_request_t;

#endif
