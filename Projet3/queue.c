#include <string.h>
#include "queue.h"

#define MAX_LENGTH 256

queue_t *createQueue(unsigned capacity) {
    int i;
    queue_t *queue = (queue_t *) malloc(sizeof(queue_t));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->array = (char **) malloc(queue->capacity * sizeof(char *));

    for (i = 0; i < capacity; ++i) {
        queue->array[i] = (char*) malloc(MAX_LENGTH * sizeof(char));
    }

    return queue;
}

int isFull(queue_t *queue) {
    return (queue->size == queue->capacity);
}

int isEmpty(queue_t *queue) {
    return (queue->size == 0);
}

void enqueue(queue_t *queue, char *item) {
    if (isFull(queue))
        return;

    queue->rear = (queue->rear + 1) % queue->capacity;

    strncpy(queue->array[queue->rear], item, MAX_LENGTH - 1);
    queue->array[queue->rear][MAX_LENGTH - 1] = '\0';

    /*queue->array[queue->rear] = item;*/
    queue->size = queue->size + 1;
    /*printf("%d enqueued to queue\n", item);*/
}

char *dequeue(queue_t *queue) {
    char *item;

    if (isEmpty(queue))
        return NULL;

    item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;

    return item;
}

char *front(queue_t *queue) {
    if (isEmpty(queue))
        return NULL;

    return queue->array[queue->front];
}

char *rear(queue_t *queue) {
    if (isEmpty(queue))
        return NULL;
    return queue->array[queue->rear];
}   
