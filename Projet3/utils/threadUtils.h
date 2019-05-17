#ifndef PROJET3_THREADUTILS_H
#define PROJET3_THREADUTILS_H

#include <pthread.h>

void mutex_lock_check(pthread_mutex_t *mutex);
void mutex_unlock_check(pthread_mutex_t *mutex);
void create_thread_check(pthread_t *thread, void *(*func) (void *), void *arg);

#endif
