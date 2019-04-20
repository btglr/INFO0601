#include <string.h>
#include "queue.h"

#define MAX_LENGTH 256

queue_t *createQueue(unsigned capacity) {
    int i;
    queue_t *queue = (queue_t *) malloc(sizeof(queue_t));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->requests = (request_connect_t**) malloc(queue->capacity * sizeof(request_connect_t*));

    for (i = 0; i < capacity; ++i) {
        queue->requests[i] = (request_connect_t*) malloc(MAX_LENGTH * sizeof(request_connect_t));
    }

    return queue;
}

int isFull(queue_t *queue) {
    return (queue->size == queue->capacity);
}

int isEmpty(queue_t *queue) {
    return (queue->size == 0);
}

void enqueue(queue_t *queue, request_connect_t *item) {
    if (isFull(queue))
        return;

    queue->rear = (queue->rear + 1) % queue->capacity;

    /*strncpy(queue->requests[queue->rear], item, MAX_LENGTH - 1);
    queue->requests[queue->rear][MAX_LENGTH - 1] = '\0';*/

    queue->requests[queue->rear] = item;
    queue->size = queue->size + 1;
    /*printf("%d enqueued to queue\n", item);*/
}

request_connect_t * dequeue(queue_t *queue) {
    request_connect_t *item;

    if (isEmpty(queue))
        return NULL;

    item = queue->requests[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;

    return item;
}

request_connect_t * front(queue_t *queue) {
    if (isEmpty(queue))
        return NULL;

    return queue->requests[queue->front];
}

request_connect_t * rear(queue_t *queue) {
    if (isEmpty(queue))
        return NULL;

    return queue->requests[queue->rear];
}   
