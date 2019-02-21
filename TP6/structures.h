#ifndef TP6_STRUCTURES_H
#define TP6_STRUCTURES_H

#define MESSAGE_LENGTH 512
#define REQUEST_TYPE 1

/* Structure utilisee pour les requetes */
typedef struct {
    long type;
    char message[MESSAGE_LENGTH];
} request_t;

/* Structure utilisee pour les reponses */
typedef struct {
    long type;
    int resultat;
} response_t;

#endif
