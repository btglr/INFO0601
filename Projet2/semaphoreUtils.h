#ifndef PROJET2_SEMAPHORESUTILS_H
#define PROJET2_SEMAPHORESUTILS_H

#define _XOPEN_SOURCE

#include <sys/types.h>
#include <curses.h>

int createSemaphores(key_t key, int nbSemaphores, int options, bool doExit);
int openSemaphores(key_t key);
void removeSemaphores(int semId);
void P(int semId, int num, size_t nbOperations);
void V(int semId, int num, size_t nbOperations);
void operate(int semId, unsigned short num, short semOp, size_t nbOperations);
void init(int semId, unsigned short *values);

#endif
