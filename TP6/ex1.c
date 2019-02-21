#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <errno.h>

#include "structures.h"

int main(int argc, char *argv[]) {
    int key, messageId;
    request_t request;
    char action;
    char *msg;

    if (argc >= 3) {
        key = atoi(argv[1]);
        action = *argv[2];

        if (action == 'E') {
            if (argc >= 4) {
                msg = (char*) malloc(strlen(argv[3]) + 1);

                if (msg == NULL) {
                    fprintf(stderr, "An error occurred during malloc\n");
                    exit(EXIT_FAILURE);
                }

                strcpy(msg, argv[3]);
            }

            else {
                fprintf(stderr, "At least 3 arguments are required\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    else {
        fprintf(stderr, "At least 2 arguments are required\n");
        exit(EXIT_FAILURE);
    }

    /* Creation de la file si elle n'existe pas */
    if((messageId = msgget((key_t) key, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if(errno == EEXIST) {
            if((messageId = msgget((key_t) key, 0)) == -1) {
                perror("An error occurred while getting the queue");
                exit(EXIT_FAILURE);
            }
        }

        else {
            perror("An error occurred while creating the queue");
            exit(EXIT_FAILURE);
        }
    }

    request.type = REQUEST_TYPE;

    switch (action) {
        case 'R':
            printf("Receiving message...\n");

            if (msgrcv(messageId, &request, sizeof(request_t) - sizeof(long), REQUEST_TYPE, 0) == -1) {
                perror("An error occurred while receiving the request");
                exit(EXIT_FAILURE);
            }

            printf("Received message: %s\n", request.message);

            break;

        case 'E':
            printf("Sending message: %s\n", msg);

            strcpy(request.message, msg);

            if(msgsnd(messageId, &request, sizeof(request_t) - sizeof(long), 0) == -1) {
                perror("An error occurred while sending the request");
                exit(EXIT_FAILURE);
            }
            break;

        case 'S':
            printf("Deleting queue...\n");

            /* Recuperation de la file */
            if((messageId = msgget((key_t) key, 0)) == -1) {
                perror("An error occurred while getting the queue");
                exit(EXIT_FAILURE);
            }

            /* Suppression de la file */
            if(msgctl(messageId, IPC_RMID, 0) == -1) {
                perror("An error occurred while deleting the queue");
                exit(EXIT_FAILURE);
            }
            break;
    }

    return EXIT_SUCCESS;
}