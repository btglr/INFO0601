#include "windowUpdater.h"
#include "structures/updateQueue.h"
#include "utils/threadUtils.h"
#include <stdlib.h>

extern pthread_mutex_t displayMutex;

pthread_mutex_t displayMutex;

void *updater(void *arg) {
    updateQueue_t *queue = (updateQueue_t*) arg;
    updateMessage_t *message;

    while (1) {
        mutex_lock_check(&queue->mutex);

        while (isUpdateQueueEmpty(queue)) {
            pthread_cond_wait(&queue->cond, &queue->mutex);
        }

        message = dequeueUpdate(queue);

        mutex_lock_check(&displayMutex);
        message->to_run(message->args);
        mutex_unlock_check(&displayMutex);

        mutex_unlock_check(&queue->mutex);
    }

    return NULL;
}
