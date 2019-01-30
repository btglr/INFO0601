/**
 * @file fileUtils.h
 * @author Bastien T.
 * @date 21 January 2018
 * @brief Utility functions for files
 */

#ifndef TP3_UTILS_H
#define TP3_UTILS_H

#include <sys/types.h>

/**
 * Returns a concatenation of the folder and the filename
 * @param folder The folder where resides the file
 * @param filename The file's name
 * @return A new char of the format folder/filename
 */
char* getPath(char* folder, char *filename);

/**
 * Makes a copy of the file at @p src to @p dest
 * @param src The filename of the source
 * @param dest The filename of the destination
 * @return The number of bytes written
 */
ssize_t copyFile(char *src, char *dest);

/**
 * Opens the file pointed by @p filename with the given @p flags
 * @param filename The filename
 * @param flags The flags to open the file with
 * @param ... If used with O_CREAT, the mode to set the file with
 * @return The file descriptor pointing to the file
 */
int openFile(char *folder, char *filename, int flags, ...);

/**
 * Seeks at the @p offset starting either from the beginning, current position or end
 * @param fd The file descriptor of the file to seek into
 * @param offset The offset at which to start from
 * @param from The position from which to seek
 */
void seekFile(int fd, off_t offset, int from);

/**
 * Writes the given @p buf into the file
 * @param fd The file descriptor
 * @param buf The buffer to write
 * @param length The length of the buffer
 * @return The number of bytes written
 */
ssize_t writeFile(int fd, void *buf, size_t length);

/**
 * Seeks at the @p offset starting from the beginning, current position or end, then writes the given @p buf into the file
 * @param fd The file descriptor
 * @param buf The buffer to write
 * @param offset The offset at which to start from
 * @param from The position from which to seek
 * @param length The length of the buffer
 * @return The number of bytes written
 */
ssize_t writeFileOff(int fd, void *buf, off_t offset, int from, size_t length);

/**
 * Reads @p length bytes into the given buffer
 * @param fd The file descriptor
 * @param buf The buffer that will contain the read bytes
 * @param length The number of bytes to read
 * @return The number of bytes actually read
 */
ssize_t readFile(int fd, void *buf, size_t length);

/**
 * Seeks at the @p offset starting from the beginning, current position or end, then reads @p length bytes into the given buffer
 * @param fd The file descriptor
 * @param buf The buffer that will contain the read bytes
 * @param offset The offset at which to start from
 * @param from The position from which to seek
 * @param length The number of bytes to read
 * @return The number of bytes actually read
 */
ssize_t readFileOff(int fd, void *buf, off_t offset, int from, size_t length);

/**
 * Closes a file
 * @param fd The file descriptor
 */
void closeFile(int fd);

#endif
