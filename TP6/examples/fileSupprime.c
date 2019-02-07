/**
 * Ce programme permet de supprimer la file de messages qui a ete creee par
 * le programme 'fileServeur'.
 * @author Cyril Rabat
 * @version 07/02/2014
 **/
#define _XOPEN_SOURCE

#include <stdlib.h>     /* Pour exit, EXIT_FAILURE, EXIT_SUCCESS */
#include <stdio.h>      /* Pour printf, perror */
#include <sys/msg.h>    /* Pour msgget, msgctl */

#include "structures.h"

int main() {
    int msqid;

    /* Recuperation de la file */
    if((msqid = msgget((key_t)CLE, 0)) == -1) {
        perror("Erreur lors de la recuperation de la file ");
        exit(EXIT_FAILURE);
    }

    /* Suppression de la file */
    if(msgctl(msqid, IPC_RMID, 0) == -1) {
        perror("Erreur lors de la suppression de la file ");
        exit(EXIT_FAILURE);
    }

    printf("File supprimee.\n");

    return EXIT_SUCCESS;
}

