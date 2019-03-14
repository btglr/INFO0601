#ifndef TP9_QUEUE_H
#define TP9_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct queue {
    int front, rear, size;
    unsigned capacity;
    char** array;
} queue_t;

queue_t* createQueue(unsigned capacity);
int isFull(queue_t* queue);
int isEmpty(queue_t* queue);
void enqueue(queue_t *queue, char *item);
char * dequeue(queue_t *queue);
char * front(queue_t *queue);
char * rear(queue_t *queue);

#endif
