#include "semaphoreUtils.h"
#include "ncurses.h"

#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <stdlib.h>

int createSemaphores(key_t key, int nbSemaphores, int options, bool doExit) {
    int semId;

    if ((semId = semget(key, nbSemaphores, options)) == -1) {
        if (errno == EEXIST) {
            if (doExit) {
                stop_ncurses();
                perror("An error occurred while trying to create the array of semaphores");
                exit(EXIT_FAILURE);
            }

            else {
                semId = -2;
            }
        }

        else {
            stop_ncurses();
            perror("An error occurred while trying to create the array of semaphores");
            exit(EXIT_FAILURE);
        }
    }

    return semId;
}

int openSemaphores(key_t key) {
    int semId;

    if ((semId = semget(key, 0, 0)) == -1) {
        stop_ncurses();
        perror("An error occurred while trying to open the array of semaphores");
        exit(EXIT_FAILURE);
    }

    return semId;
}

void removeSemaphores(int semId) {
    if (semctl(semId, 0, IPC_RMID) == -1) {
        stop_ncurses();
        perror("An error occurred while trying to remove the array of semaphores");
        exit(EXIT_FAILURE);
    }
}

/*void P(int semId, int num, ...) {
    size_t nbOperations;
    va_list va;
    va_start(va, num);

    nbOperations = va_arg(va, size_t);
    nbOperations = nbOperations == 0 ? 1 : nbOperations;

    printf("P(S%d, %ld)\n", num, nbOperations);
    operate(semId, (unsigned short) num, -1, nbOperations);

    va_end(va);
}

void V(int semId, int num, ...) {
    size_t nbOperations;
    va_list va;
    va_start(va, num);

    nbOperations = va_arg(va, size_t);
    nbOperations = nbOperations == 0 ? 1 : nbOperations;

    printf("V(S%d, %ld)\n", num, nbOperations);
    operate(semId, (unsigned short) num, 1, nbOperations);

    va_end(va);
}*/

void P(int semId, int num, size_t nbOperations) {
    printf("P(S%d, %ld)\n", num, nbOperations);
    operate(semId, (unsigned short) num, -1, nbOperations);
}

void V(int semId, int num, size_t nbOperations) {
    printf("V(S%d, %ld)\n", num, nbOperations);
    operate(semId, (unsigned short) num, 1, nbOperations);
}

void operate(int semId, unsigned short num, short semOp, size_t nbOperations) {
    size_t i;
    struct sembuf *op = (struct sembuf*) malloc(nbOperations * sizeof(struct sembuf));

    if (nbOperations > 1) {

        for (i = 0; i < nbOperations; ++i) {
            op[i].sem_num = num;
            op[i].sem_op = semOp;
            op[i].sem_flg = 0;
        }
    }

    else {
        op->sem_num = num;
        op->sem_op = semOp;
        op->sem_flg = 0;
    }

    if(semop(semId, op, nbOperations) == -1) {
        stop_ncurses();
        perror("An error occurred while trying to operate on the semaphore");
        exit(EXIT_FAILURE);
    }
}

void init(int semId, unsigned short *values) {
    if(semctl(semId, 0, SETALL, values) == -1) {
        stop_ncurses();
        perror("An error occurred while trying to initialize the semaphores");
        exit(EXIT_FAILURE);
    }
}
