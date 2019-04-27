#include <string.h>
#include "queue.h"
#include "memoryUtils.h"

queue_t *createQueue(unsigned capacity) {
    queue_t *queue = (queue_t *) malloc_check(sizeof(queue_t));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->requests = (queue_element_t**) malloc_check(queue->capacity * sizeof(queue_element_t*));

    /*for (i = 0; i < capacity; ++i) {
        queue->requests[i] = (queue_element_t*) malloc(MAX_LENGTH * sizeof(queue_element_t));
    }*/

    return queue;
}

void destroyQueue(queue_t *queue) {
/*
    for (i = 0; i < queue->capacity; ++i) {
        free(queue->requests[i]);
    }
*/

    free(queue->requests);
    free(queue);
}

int isFull(queue_t *queue) {
    return (queue->size == queue->capacity);
}

int isEmpty(queue_t *queue) {
    return (queue->size == 0);
}

void enqueue(queue_t *queue, queue_element_t *item) {
    if (isFull(queue))
        return;

    queue->rear = (queue->rear + 1) % queue->capacity;

    /*strncpy(queue->requests[queue->rear], item, MAX_LENGTH - 1);
    queue->requests[queue->rear][MAX_LENGTH - 1] = '\0';*/

    queue->requests[queue->rear] = item;
    queue->size = queue->size + 1;
    /*printf("%d enqueued to queue\n", item);*/
}

queue_element_t * dequeue(queue_t *queue) {
    queue_element_t *item;

    if (isEmpty(queue))
        return NULL;

    item = queue->requests[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;

    return item;
}

queue_element_t * front(queue_t *queue) {
    if (isEmpty(queue))
        return NULL;

    return queue->requests[queue->front];
}

queue_element_t * rear(queue_t *queue) {
    if (isEmpty(queue))
        return NULL;

    return queue->requests[queue->rear];
}
