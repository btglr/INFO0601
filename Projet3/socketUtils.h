#ifndef PROJET3_SOCKETUTILS_H
#define PROJET3_SOCKETUTILS_H

#include <arpa/inet.h>

int createSocket(int type, int protocol);
void initAddress(struct sockaddr_in *address, int port, char* ipAddress);
void initAddressAny(struct sockaddr_in *address, int port);

#endif
