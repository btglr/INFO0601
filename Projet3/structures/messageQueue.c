#include <string.h>
#include "messageQueue.h"
#include "../utils/memoryUtils.h"

messageQueue_t *createQueue(unsigned capacity) {
    messageQueue_t *queue = (messageQueue_t *) malloc_check(sizeof(messageQueue_t));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->requests = (queue_element_t**) malloc_check(queue->capacity * sizeof(queue_element_t*));

    /*for (i = 0; i < capacity; ++i) {
        messageQueue->requests[i] = (queue_element_t*) malloc(MAX_LENGTH * sizeof(queue_element_t));
    }*/

    return queue;
}

void destroyQueue(messageQueue_t *queue) {
    queue_element_t *elem;
/*
    for (i = 0; i < messageQueue->capacity; ++i) {
        free(messageQueue->requests[i]);
    }
*/

    while (!isEmpty(queue)) {
        elem = dequeue(queue);
        free(elem->request);
        free(elem->sourceAddr);
        free(elem);
    }

    free(queue->requests);
    free(queue);
}

int isFull(messageQueue_t *queue) {
    return (queue->size == queue->capacity);
}

int isEmpty(messageQueue_t *queue) {
    return (queue->size == 0);
}

void enqueue(messageQueue_t *queue, queue_element_t *item) {
    if (isFull(queue))
        return;

    queue->rear = (queue->rear + 1) % queue->capacity;

    /*strncpy(messageQueue->requests[messageQueue->rear], item, MAX_LENGTH - 1);
    messageQueue->requests[messageQueue->rear][MAX_LENGTH - 1] = '\0';*/

    queue->requests[queue->rear] = item;
    queue->size = queue->size + 1;
    /*printf("%d enqueued to messageQueue\n", item);*/
}

queue_element_t * dequeue(messageQueue_t *queue) {
    queue_element_t *item;

    if (isEmpty(queue))
        return NULL;

    item = queue->requests[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;

    return item;
}

queue_element_t * front(messageQueue_t *queue) {
    if (isEmpty(queue))
        return NULL;

    return queue->requests[queue->front];
}

queue_element_t * rear(messageQueue_t *queue) {
    if (isEmpty(queue))
        return NULL;

    return queue->requests[queue->rear];
}
