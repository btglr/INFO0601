#ifndef PROJET2_SHAREDMEMORYUTILS_H
#define PROJET2_SHAREDMEMORYUTILS_H

#define _XOPEN_SOURCE

#include <curses.h> /* For the bool type */
#include <sys/types.h> /* For the key_t, ssize_t types */
#include <sys/shm.h> /* For the shm functions */

/**
 * Creates a shared memory segment with the given options
 * @param key The segment's key
 * @param size The size of the shared memory segment
 * @param options The options to create it with
 * @param doExit Should the function exit if the message queue already exists?
 * @return The segment's id
 */
int createSegment(key_t key, size_t size, int options, bool doExit);

/**
 * Opens a shared memory segment with the given key
 * @param key The segment's key
 * @return The segment's id
 */
int openSegment(key_t key);

/**
 * Attaches the shared memory segment to the current process
 * @param shmId The segment's id
 * @param options The options to attach the segment with
 */
void* attachSegment(int shmId, int options);

/**
 * Detaches the shared memory segment
 * @param shmAddr The attached address (returned by attachSegment)
 */
void detachSegment(void *shmAddr);

/**
 * Removes the shared memory segment
 * @param shmId The shared memory segment's id to remove
 */
void removeSegment(int shmId);

#endif
