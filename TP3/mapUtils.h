#ifndef TP3_MAPUTILS_H
#define TP3_MAPUTILS_H

unsigned char getTotalLives(int fd);
unsigned char getRemainingLives(int fd);
int getMapVersion(int fd);
ssize_t setRemainingLives(int fd, unsigned char lives);

void increaseTotalLives(int fd);
void decreaseTotalLives(int fd);
int getWallCount(int fd, int type);
void increaseMapVersion(int fd);
unsigned char getWallAt(int fd, int x, int y);
unsigned char setWallAt(int fd, int x, int y, unsigned char type);
unsigned char getNextWallAt(int fd, int x, int y, int editor);
int makeMultipleOf(int v, int m);

#endif
