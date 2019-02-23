#ifndef PROJET2_MESSAGEQUEUEUTILS_H
#define PROJET2_MESSAGEQUEUEUTILS_H

#define _XOPEN_SOURCE

#include <curses.h> /* For the bool type */
#include <sys/types.h> /* For the key_t, ssize_t types */

/**
 * Creates a message queue with the given options
 * @param key The message queue's key
 * @param options The options to create it with
 * @param doExit Should the function exit if the message queue already exists?
 * @return The message queue's id
 */
int createMessageQueue(key_t key, int options, bool doExit);

/**
 * Opens a message queue with the given key
 * @param key The message queue's key
 * @return The message queue's id
 */
int openMessageQueue(key_t key);

/**
 * Sends a message into the given queue
 * @param queueId The message queue's id
 * @param msg The structure containing the data
 * @param size The TOTAL size of the structure
 * @param options The options to send the message with
 */
void sendMessage(int queueId, void *msg, size_t size, int options);

/**
 * Reads a message into the given structure
 * @param queueId The message queue's id
 * @param msg The structure that will contain the data
 * @param size The TOTAL size of the structure
 * @param type The (long) type of the message to receive
 * @param options The options to receive the message with
 * @return The number of bytes read
 */
ssize_t receiveMessage(int queueId, void *msg, size_t size, long type, int options);

/**
 * Removes the given queue
 * @param queueId The message queue's id to remove
 */
void removeMessageQueue(int queueId);

#endif
