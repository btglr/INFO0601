#ifndef TP3_UTILS_H
#define TP3_UTILS_H

#include <sys/types.h>

void seekFile(int fd, off_t offset, int from);
void writeFile(int fd, void *buf, size_t length);
void writeFileOff(int fd, void *buf, off_t offset, size_t length);
ssize_t readFile(int fd, void *buf, size_t length);
ssize_t readFileOff(int fd, void *buf, off_t offset, size_t length);
void closeFile(int fd);
/*void test(int fd);*/

#endif
