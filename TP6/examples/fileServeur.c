/**
 * Ce programme vise a illustrer le fonctionnement d'une file de messages
 * IPC System V. Le serveur cree une file de messages et se met en attente
 * d'une requete. Des qu'il en recoit une, il envoie une reponse.
 * @author Cyril Rabat
 * @version 07/02/2014
 **/
#define _XOPEN_SOURCE

#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, perror */
#include <sys/msg.h>    /* Pour msgget, msgsnd, msgrcv */
#include <errno.h>      /* Pour errno */
#include <sys/stat.h>   /* Pour S_IRUSR, S_IWUSR */

#include "structures.h"

int main() {
    int msqid;
    request_t requete;
    response_t reponse;

    /* Creation de la file si elle n'existe pas */
    if ((msqid = msgget((key_t) CLE, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if (errno == EEXIST)
            fprintf(stderr, "Erreur : file (cle=%d) existante\n", CLE);
        else
            perror("Erreur lors de la creation de la file ");
        exit(EXIT_FAILURE);
    }

    /* Attente d'une requete */
    printf("Serveur : en attente d'une requete...\n");
    if (msgrcv(msqid, &requete, sizeof(request_t) - sizeof(long), TYPE_REQUETE, 0) == -1) {
        perror("Erreur lors de la reception d'une requete ");
        exit(EXIT_FAILURE);
    }

    printf("Serveur : requete recue (%d, %d)\n", requete.valeur1, requete.valeur2);

    /* Envoi de la reponse */
    reponse.type = TYPE_REPONSE;
    reponse.resultat = requete.valeur1 + requete.valeur2;

    if (msgsnd(msqid, &reponse, sizeof(response_t) - sizeof(long), 0) == -1) {
        perror("Erreur lors de l'envoi de la reponse ");
        exit(EXIT_FAILURE);
    }
    printf("Serveur : reponse envoyee.\n");

    return EXIT_SUCCESS;
}
