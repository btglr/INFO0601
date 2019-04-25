#ifndef TP9_QUEUE_H
#define TP9_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "structures.h"

typedef struct queue {
    int front, rear, size;
    unsigned capacity;
    queue_element_t** requests;
} queue_t;

queue_t* createQueue(unsigned capacity);
int isFull(queue_t* queue);
int isEmpty(queue_t* queue);
void enqueue(queue_t *queue, queue_element_t *item);
queue_element_t * dequeue(queue_t *queue);
queue_element_t * front(queue_t *queue);
queue_element_t * rear(queue_t *queue);
void destroyQueue(queue_t* queue);

#endif
