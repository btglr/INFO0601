#undef _GNU_SOURCE
#define _GNU_SOURCE

#include "structures.h"
#include "messageQueueUtils.h"
#include "semaphoreUtils.h"
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

bool run = TRUE;

void handler(int sig) {
    if (sig == SIGINT) {
        run = FALSE;
    }
}

int main() {
    int queueId, semId, shmId;
    request_t request;
    response_t response;
    struct sigaction action;

    sigemptyset(&action.sa_mask);
    action.sa_handler = handler;

    if (sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "An error occurred while catching the SIGINT signal\n");
        exit(EXIT_FAILURE);
    }

    request.type = GET_KEYS_TYPE;
    queueId = openMessageQueue(KEY_MESSAGE_QUEUE);
    sendMessage(queueId, &request, sizeof(request_t), 0);

    receiveMessage(queueId, &response, sizeof(response_t), RESPONSE_TYPE, 0);

    semId = response.keySem;
    shmId = response.keyShm;

    printf("Sem id: %d, Shm id: %d\n", semId, shmId);

    P(semId, SEM_MINOTAUR, 1);

    printf("Semaphore value: %d\n", getSemaphoreValue(semId, SEM_MINOTAUR));

    request.type = CONNECT_TYPE;
    request.message.connect.pid = getpid();
    request.message.connect.programType = MINOTAUR;
    sendMessage(queueId, &request, sizeof(request_t), 0);

    while (1) {
        if (!run) {
            break;
        }

        sleep(SLEEP_MINOTAUR);
    }

    V(semId, SEM_MINOTAUR, 1);

    request.type = DISCONNECT_TYPE;
    request.message.disconnect.pid = getpid();
    request.message.disconnect.programType = MINOTAUR;
    sendMessage(queueId, &request, sizeof(request_t), 0);

    return EXIT_SUCCESS;
}
