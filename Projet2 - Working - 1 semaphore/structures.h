#ifndef PROJET2_STRUCTURES_H
#define PROJET2_STRUCTURES_H

#include "constants.h"
#include <unistd.h>

typedef struct {
    int programType;
    pid_t pid;
} connect_t;

typedef struct {
    int programType;
    pid_t pid;
} disconnect_t;

typedef struct {
    long type;
    int shmId;
    int semId;
} response_t;

typedef struct {
    long type;
    union {
        connect_t connect;
        disconnect_t disconnect;
    } message;
} request_t;

typedef struct {
    int mapVersion;
    unsigned char lives;
    unsigned char livesLeft;
    unsigned char posX;
    unsigned char posY;
    unsigned char map[MAP_WIDTH * MAP_HEIGHT];
    /* If % sizeof(int) */
#if MAP_WIDTH * MAP_HEIGHT % 4 > 0
    char _pad2[MAP_WIDTH / MAP_HEIGHT];
#endif
} map_t;

#endif
