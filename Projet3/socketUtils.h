#ifndef PROJET3_SOCKETUTILS_H
#define PROJET3_SOCKETUTILS_H

#include <arpa/inet.h>
#include <unistd.h>

void bindAddress(int sock, const void *addr);
int createSocket(int type, int protocol);
void initAddress(struct sockaddr_in *address, int port, char* ipAddress);
void initAddressAny(struct sockaddr_in *address, int port);
ssize_t sendUDP(int sock, const void *msg, size_t msgLength, const void *destAddr, socklen_t addrLength);
ssize_t receiveUDP(int sock, void *msg, size_t msgLength, int flags, void *srcAddr, socklen_t *addrLength);
void listenSocket(int sock, int nbQueue);
int acceptSocket(int sock);
void connectSocket(int sock, const void *addr);
void str_addr_to_network(int family, const char *addr, void *addrBuffer);
void network_addr_to_str(int family, const void *addr, char *addrBuffer, socklen_t len);

#endif
