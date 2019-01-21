#ifndef TP3_UTILS_H
#define TP3_UTILS_H

#include <sys/types.h>

int copyFile(char *src, char *dest);
int openFile(char *filename, int flags, ...);
void seekFile(int fd, off_t offset, int from);
ssize_t writeFile(int fd, void *buf, size_t length);
ssize_t writeFileOff(int fd, void *buf, off_t offset, size_t length);
ssize_t readFile(int fd, void *buf, size_t length);
ssize_t readFileOff(int fd, void *buf, off_t offset, size_t length);
void closeFile(int fd);
/*void test(int fd);*/

#endif
