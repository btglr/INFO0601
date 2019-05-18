#include <curses.h>
#include "../structures/structures.h"

#ifndef PROJET3_CHUNKMANAGER_H
#define PROJET3_CHUNKMANAGER_H

int getChunk(coord_t *coords, map_t *map);
void updateChunk(WINDOW *window, map_t *map, int chunkId);
chunk_t* createChunks(int mapWidth, int mapHeight, chunk_size_t chunkSize);
chunk_size_t determineChunkSize(int mapWidth, int mapHeight);
void populateChunks(const char *mapBuffer, map_t *map);
int globalToLocalCoordinate(coord_t *global, chunk_size_t chunkSize);
int flattenCoordinate(coord_t *coord, int width);
void lockAllChunks(map_t *map);
void unlockAllChunks(map_t *map);

#endif
