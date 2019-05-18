#include <string.h>
#include <pthread.h>
#include "../utils/memoryUtils.h"
#include "updateQueue.h"

updateQueue_t *createUpdateQueue(unsigned capacity) {
    updateQueue_t *queue = (updateQueue_t *) malloc_check(sizeof(updateQueue_t));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->messages = (updateMessage_t**) malloc_check(sizeof(updateMessage_t*) * queue->capacity);
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond, NULL);

    return queue;
}

void destroyUpdateQueue(updateQueue_t *queue) {
    updateMessage_t *elem;

    while (!isUpdateQueueEmpty(queue)) {
        elem = dequeueUpdate(queue);
        free(elem);
    }

    free(queue);
}

int isUpdateQueueFull(updateQueue_t *queue) {
    return (queue->size == queue->capacity);
}

int isUpdateQueueEmpty(updateQueue_t *queue) {
    return (queue->size == 0);
}

void enqueueMessage(updateQueue_t *queue, void *(*to_run) (void *), void *args) {
    updateMessage_t *item;

    if (isUpdateQueueFull(queue))
        return;

    item = (updateMessage_t*) malloc(sizeof(updateMessage_t));
    item->to_run = to_run;
    item->args = args;

    enqueueUpdate(queue, item);
}

void enqueueUpdate(updateQueue_t *queue, updateMessage_t *item) {
    if (isUpdateQueueFull(queue))
        return;

    queue->rear = (queue->rear + 1) % queue->capacity;

    queue->messages[queue->rear] = item;
    queue->size = queue->size + 1;
}

updateMessage_t *dequeueUpdate(updateQueue_t *queue) {
    updateMessage_t *item;

    if (isUpdateQueueEmpty(queue))
        return NULL;

    item = queue->messages[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;

    return item;
}

updateMessage_t *frontUpdateQueue(updateQueue_t *queue) {
    if (isUpdateQueueEmpty(queue))
        return NULL;

    return queue->messages[queue->front];
}

updateMessage_t *rearUpdateQueue(updateQueue_t *queue) {
    if (isUpdateQueueEmpty(queue))
        return NULL;

    return queue->messages[queue->rear];
}
