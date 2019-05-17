#include <stdio.h>
#include <stdlib.h>
#include "threadUtils.h"
#include "ncurses.h"

void mutex_lock_check(pthread_mutex_t *mutex) {
    int returnValue;

    if ((returnValue = pthread_mutex_lock(mutex)) != 0) {
        stop_ncurses();
        fprintf(stderr, "An error occurred while trying to lock the mutex: error %d\n", returnValue);
        exit(EXIT_FAILURE);
    }
}

void create_thread_check(pthread_t *thread, void *(*func)(void *), void *arg) {
    int returnValue;

    if ((returnValue = pthread_create(thread, NULL, func, arg)) != 0) {
        stop_ncurses();
        fprintf(stderr, "An error occurred while creating the thread: error %d\n", returnValue);
        exit(EXIT_FAILURE);
    }
}

void mutex_unlock_check(pthread_mutex_t *mutex) {
    int returnValue;

    if ((returnValue = pthread_mutex_unlock(mutex)) != 0) {
        stop_ncurses();
        fprintf(stderr, "An error occurred while trying to unlock the mutex, error: %d\n", returnValue);
        exit(EXIT_FAILURE);
    }
}
