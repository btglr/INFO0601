#include "sharedMemoryUtils.h"
#include "ncurses.h"
#include <stdlib.h>
#include <errno.h>

void *attachSegment(int shmId, int options) {
    void *address;

    if ((address = shmat(shmId, NULL, options)) == (void*) -1) {
        stop_ncurses();
        perror("An error occurred while trying to attach to the shared memory segment");
        exit(EXIT_FAILURE);
    }

    return address;
}

int createSegment(key_t key, size_t size, int options, bool doExit) {
    int shmId;

    if ((shmId = shmget(key, size, options)) == -1) {
        if (errno == EEXIST) {
            if (doExit) {
                stop_ncurses();
                perror("An error occurred while trying to create the shared memory segment");
                exit(EXIT_FAILURE);
            }

            else {
                shmId = -2;
            }
        }

        else {
            stop_ncurses();
            perror("An error occurred while trying to create the shared memory segment");
            exit(EXIT_FAILURE);
        }
    }

    return shmId;
}

int openSegment(key_t key) {
    int shmId;

    if ((shmId = shmget(key, 0, 0)) == -1) {
        stop_ncurses();
        perror("An error occurred while trying to open the shared memory segment");
        exit(EXIT_FAILURE);
    }

    return shmId;
}

void detachSegment(void *shmAddr) {
    if (shmdt(shmAddr) == -1) {
        stop_ncurses();
        perror("An error occurred while trying to detach the shared memory segment");
        exit(EXIT_FAILURE);
    }
}

void removeSegment(int shmId) {
    if (shmctl(shmId, IPC_RMID, NULL) == -1) {
        stop_ncurses();
        perror("An error occurred while trying to remove the shared memory segment");
        exit(EXIT_FAILURE);
    }
}
