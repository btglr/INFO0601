/**
 * Ce programme vise a illustrer le fonctionnement d'une file de messages
 * IPC System V. Le client envoie une requete dans la file de messages et se
 * met en attente d'une reponse.
 * @author Cyril Rabat
 * @version 07/02/2014
 **/
#define _XOPEN_SOURCE

#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, perror */
#include <sys/msg.h>    /* Pour msgget, msgsnd, msgrcv */
#include <errno.h>      /* Pour errno */

#include "structures.h"

int main() {
    int msqid;
    request_t requete;
    response_t reponse;

    /* Recuperation de la file */
    if((msqid = msgget((key_t)CLE, 0)) == -1) {
        perror("Erreur lors de la recuperation de la file ");
        exit(EXIT_FAILURE);
    }

    /* Envoi d'une requete */
    requete.type = TYPE_REQUETE;
    requete.valeur1 = 3;
    requete.valeur2 = 6;

    if(msgsnd(msqid, &requete, sizeof(request_t) - sizeof(long), 0) == -1) {
        perror("Erreur lors de l'envoi de la requete ");
        exit(EXIT_FAILURE);
    }
    printf("Client : envoi d'une requete.\n");

    /* Reception de la reponse */
    printf("Client : attente de la reponse...\n");
    if(msgrcv(msqid, &reponse, sizeof(response_t) - sizeof(long), TYPE_REPONSE, 0) == -1) {
        perror("Erreur lors de la reception de la reponse ");
        exit(EXIT_FAILURE);
    }

    printf("Client : le resultat est %d\n", reponse.resultat);

    return EXIT_SUCCESS;
}

