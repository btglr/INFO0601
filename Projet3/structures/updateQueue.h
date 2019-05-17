#ifndef TP9_QUEUE_H
#define TP9_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <curses.h>
#include "structures.h"

typedef struct {
    WINDOW *window;
    int chunkId;
} updateMessage_t;

typedef struct updateQueue {
    int front, rear, size;
    unsigned capacity;
    updateMessage_t **messages;
    pthread_mutex_t mutex;
} updateQueue_t;

updateQueue_t* createUpdateQueue(unsigned capacity);
int isUpdateQueueFull(updateQueue_t* queue);
int isUpdateQueueEmpty(updateQueue_t* queue);
void enqueueUpdate(updateQueue_t *queue, updateMessage_t *item);
updateMessage_t *dequeueUpdate(updateQueue_t *queue);
updateMessage_t *frontUpdateQueue(updateQueue_t *queue);
updateMessage_t *rearUpdateQueue(updateQueue_t *queue);
void destroyUpdateQueue(updateQueue_t* queue);

#endif
