#include "messageQueueUtils.h"
#include "ncurses.h"
#include "sharedMemoryUtils.h"

#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int openMessageQueue(key_t key) {
    int queueId;

    if ((queueId = msgget(key, 0)) == -1) {
        stop_ncurses();
        perror("An error occurred while trying to open a message queue");
        exit(EXIT_FAILURE);
    }

    return queueId;
}

void sendMessage(int queueId, void *msg, size_t size, int options) {
    if (msgsnd(queueId, msg, size - sizeof(long), options) == -1) {
        stop_ncurses();
        perror("An error occurred while trying to send a message through the queue");
        exit(EXIT_FAILURE);
    }
}

ssize_t receiveMessage(int queueId, void *msg, size_t size, long type, int options) {
    ssize_t readBytes;

    if ((readBytes = msgrcv(queueId, msg, size - sizeof(long), type, options)) == -1) {
        stop_ncurses();
        perror("An error occurred while trying to get a message from the queue");
        exit(EXIT_FAILURE);
    }

    return readBytes;
}

void removeMessageQueue(int queueId) {
    if(msgctl(queueId, IPC_RMID, 0) == -1) {
        stop_ncurses();
        perror("An error occurred while trying to delete the message queue");
        exit(EXIT_FAILURE);
    }
}

int createMessageQueue(key_t key, int options, bool doExit) {
    int queueId;

    if ((queueId = msgget(key, options)) == -1) {
        if (errno == EEXIST) {
            if (doExit) {
                stop_ncurses();
                perror("An error occurred while trying to create a message queue");
                exit(EXIT_FAILURE);
            }

            else {
                queueId = -2;
            }
        }

        else {
            stop_ncurses();
            perror("An error occurred while trying to create a message queue");
            exit(EXIT_FAILURE);
        }
    }

    return queueId;
}
