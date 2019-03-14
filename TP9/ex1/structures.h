#ifndef TP9_STRUCTURES_H
#define TP9_STRUCTURES_H

#define MESSAGE 0
#define REQUEST 1

typedef struct {
    unsigned char type;
    char message[256];
} message_t;

#endif
