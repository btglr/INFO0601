#ifndef TP9_QUEUE_H
#define TP9_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "structures.h"

typedef struct messageQueue {
    int front, rear, size;
    unsigned capacity;
    queue_element_t** requests;
} messageQueue_t;

messageQueue_t* createQueue(unsigned capacity);
int isFull(messageQueue_t* queue);
int isEmpty(messageQueue_t* queue);
void enqueue(messageQueue_t *queue, queue_element_t *item);
queue_element_t * dequeue(messageQueue_t *queue);
queue_element_t * front(messageQueue_t *queue);
queue_element_t * rear(messageQueue_t *queue);
void destroyQueue(messageQueue_t* queue);

#endif
