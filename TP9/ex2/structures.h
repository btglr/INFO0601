#ifndef TP9_STRUCTURES_H
#define TP9_STRUCTURES_H

#define TYPE_CONNECTION 0
#define TYPE_DOWNLOAD 1
#define TYPE_UPLOAD 2
#define TYPE_DELETE 3
#define TYPE_LIST 4

typedef struct {
    unsigned char type;
} request_t;

typedef struct {
    char *response;
} response_t;

#endif
