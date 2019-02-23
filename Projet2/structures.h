#ifndef PROJET2_STRUCTURES_H
#define PROJET2_STRUCTURES_H

#include "constants.h"

typedef struct {
    long type;
} request_t;

typedef struct {
    int mapVersion;
    unsigned char lives;
    char _pad1[3];
    unsigned char map[MAP_WIDTH * MAP_HEIGHT];
#if MAP_WIDTH * MAP_HEIGHT % 4 > 0
    char _pad2[MAP_WIDTH / MAP_HEIGHT];
#endif
} map_t;

#endif
