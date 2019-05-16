#include "utils.h"
#include "../structures/constants.h"
#include "fileUtils.h"
#include "ncurses.h"
#include "windowDrawer.h"
#include <string.h>
#include <fcntl.h>
#include <curses.h>
#include "chunkManager.h"
#include "memoryUtils.h"
#include <stdlib.h>
#include <pthread.h>

/**
 * Gets the chunk corresponding to the given coordinates
 * @param coords The coordinates to find the chunk of
 * @param map The map containing the chunks
 * @return The id of the chunk
 */
int getChunk(coord_t *coords, map_t *map) {
    int chunkX, chunkY;

    if (map == NULL) {
        fprintf(stderr, "Error: The map parameter is NULL\n");
        exit(EXIT_FAILURE);
    }

    if (coords == NULL) {
        fprintf(stderr, "Error: The coordinates parameter is NULL\n");
        exit(EXIT_FAILURE);
    }

    chunkX = coords->x / map->chunkSize.width;
    chunkY = coords->y / map->chunkSize.height;
    return (chunkY * (map->width / map->chunkSize.width)) + chunkX;
}

/**
 * Updates a chunk based on its chunk id within the map
 * @param window The window to update
 * @param map The map containing the chunks
 * @param chunkId The chunk id to update
 */
void updateChunk(WINDOW *window, map_t *map, int chunkId) {
    int x, y, currSquare;
    int xBegin, yBegin;
    int xEnd, yEnd;

    if (map == NULL) {
        fprintf(stderr, "Error: The map parameter is NULL\n");
        exit(EXIT_FAILURE);
    }

    xBegin = map->chunks[chunkId].xBegin;
    xEnd = map->chunks[chunkId].xEnd;
    yBegin = map->chunks[chunkId].yBegin;
    yEnd = map->chunks[chunkId].yEnd;

    currSquare = 0;
    for (y = yBegin; y <= yEnd; ++y) {
        fprintf(stderr, "DEBUG | ");

        for (x = xBegin; x <= xEnd; ++x) {
            fprintf(stderr, "%d ", map->chunks[chunkId].squares[currSquare].type);
            drawSquare(window, map->chunks[chunkId].squares[currSquare].type, x * SQUARE_WIDTH, y, FALSE);
            currSquare++;
        }

        fprintf(stderr, "\n");
    }

    fprintf(stderr, "DEBUG | Updated chunk %d\n", chunkId);

    wrefresh(window);
}

/**
 * Creates empty chunks based on chunk size and map dimensions
 * @param mapWidth The map width
 * @param mapHeight The map height
 * @param chunkSize The chunk size
 * @return An array of empty chunks
 */
chunk_t *createChunks(int mapWidth, int mapHeight, chunk_size_t chunkSize) {
    int i, j, id;
    int nbChunks;
    chunk_t *chunks;

    nbChunks = (mapHeight / chunkSize.height) * (mapWidth / chunkSize.width);
    chunks = (chunk_t*) malloc_check(sizeof(chunk_t) * nbChunks);

    for (i = 0; i < mapWidth / chunkSize.width; ++i) {
        for (j = 0; j < mapHeight / chunkSize.height; ++j) {
            id = (j * (mapWidth / chunkSize.width)) + i;
            chunks[id].chunkId = id;
            chunks[id].xBegin = i * chunkSize.width;
            chunks[id].yBegin = j * chunkSize.height;
            chunks[id].xEnd = chunks[id].xBegin + chunkSize.width - 1;
            chunks[id].yEnd = chunks[id].yBegin + chunkSize.height - 1;

            fprintf(stderr, "DEBUG | Creating chunk: XBeg: %d, XEnd: %d, YBeg: %d, YEnd: %d\n", chunks[id].xBegin, chunks[id].xEnd, chunks[id].yBegin, chunks[id].yEnd);

            chunks[id].squares = (square_t*) malloc_check(sizeof(square_t) * (chunkSize.height * chunkSize.width));

            if (pthread_mutex_init(&chunks[id].mutex, NULL) != 0) {
                fprintf(stderr, "An error occurred while initializing a chunk mutex\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    return chunks;
}

/**
 * Determines the "optimal" chunk size based on primality/divisors
 * @param mapWidth The map width
 * @param mapHeight The map height
 * @return A chunk_size_t object containing the computed width and height
 */
chunk_size_t determineChunkSize(int mapWidth, int mapHeight) {
    chunk_size_t chunk;

    chunk.width = isPrime(mapWidth) ? mapWidth : findDivisor(mapWidth);
    chunk.height = isPrime(mapHeight) ? mapHeight : findDivisor(mapHeight);

    return chunk;
}

/**
 * Populates the chunks of a map according to the corresponding map (char*) buffer
 * @param mapBuffer The map buffer
 * @param map The map object
 */
void populateChunks(const char *mapBuffer, map_t *map) {
    int x, y, i, currSquare;
    int bufferCoord;

    if (map == NULL || mapBuffer == NULL) {
        fprintf(stderr, "Error: The map parameter is NULL\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < map->nbChunks; ++i) {
        chunk_t *chunk = &map->chunks[i];

        currSquare = 0;

        for (y = chunk->yBegin; y <= chunk->yEnd; ++y) {
            for (x = chunk->xBegin; x <= chunk->xEnd; ++x) {
                bufferCoord = map->width * y + x;

                chunk->squares[currSquare].type = mapBuffer[bufferCoord];
                currSquare++;
            }
        }
    }
}

/**
 * Transforms a global coordinate (x, y) into a local chunk coordinate
 * @param global The global coordinate
 * @param mapWidth The map width
 * @param chunkSize The chunk width
 * @return The coordinate local to the chunk
 */
int globalToLocalCoordinate(coord_t *global, chunk_size_t chunkSize) {
    int flattened;

    coord_t *local = (coord_t*) malloc(sizeof(coord_t));
    local->x = global->x % chunkSize.width;
    local->y = global->y % chunkSize.height;

    flattened = flattenCoordinate(local, chunkSize.width);

    free(local);

    return flattened;
}

/**
 * Flattens the coordinate according to a given width
 * @param coord The coordinate to flatten
 * @return The flattened coordinate
 */
int flattenCoordinate(coord_t *coord, int width) {
    return (coord->x + (coord->y * width));
}