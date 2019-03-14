#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <errno.h>

#define KEY 302

struct sembuf op;

void P(int semId, unsigned short num) {
    op.sem_num = num;
    op.sem_op = -1;
    op.sem_flg = 0;

    if(semop(semId, &op, 1) == -1) {
        perror("An error occurred while changing the semaphore ");
        exit(EXIT_FAILURE);
    }
}

void V(int semId, unsigned short num) {
    op.sem_num = num;
    op.sem_op = 1;
    op.sem_flg = 0;

    if(semop(semId, &op, 1) == -1) {
        perror("An error occurred while changing the semaphore ");
        exit(EXIT_FAILURE);
    }
}

/**
 * Génère un entier aléatoire dans un intervalle [a, b].
 * @param a la borne minimale
 * @param b la borne maximale
 * @return l'entier aléatoire
 */
int random_(int a, int b) {
    return rand() % (b - a + 1) + a;
}

/**
 * Routine du fils 1.
 */
int fils1(int semId) {
    srand(time(NULL) + getpid());

    sleep(random_(1, 5));

    P(semId, 1);

    /* Bloc A1 */
    printf("Debut du bloc A1...\n");
    sleep(random_(1, 5));
    printf("Fin du bloc A1.\n");
    /* Fin bloc A1 */

    V(semId, 1);
    V(semId, 0);

    sleep(random_(1, 5));

    /* Bloc B1 */
    printf("Debut du bloc B1...\n");
    sleep(random_(1, 5));
    printf("Fin du bloc B1.\n");
    /* Fin bloc B1 */

    sleep(random_(1, 5));

    P(semId, 4);
    P(semId, 3);

    /* Bloc C1 */
    printf("Debut du bloc C1...\n");
    sleep(random_(1, 5));
    printf("Fin du bloc C1.\n");
    /* Fin bloc C1 */

    exit(EXIT_SUCCESS);
}

/**
 * Routine du fils 2.
 */
int fils2(int semId) {
    srand(time(NULL) + getpid());

    sleep(random_(1, 5));

    /* Bloc A2 */
    printf("Debut du bloc A2...\n");
    sleep(random_(1, 5));
    printf("Fin du bloc A2.\n");
    /* Fin bloc A2 */

    sleep(random_(1, 5));

    P(semId, 2);
    P(semId, 1);

    /* Bloc B2 */
    printf("Debut du bloc B2...\n");
    sleep(random_(1, 5));
    printf("Fin du bloc B2.\n");
    /* Fin bloc B2 */

    V(semId, 1);
    V(semId, 2);
    V(semId, 3);
    V(semId, 3);

    sleep(random_(1, 5));

    /* Bloc C2 */
    printf("Debut du bloc C2...\n");
    sleep(random_(1, 5));
    printf("Fin du bloc C2.\n");
    /* Fin bloc C2 */

    exit(EXIT_SUCCESS);
}

/**
 * Routine du fils 3.
 */
int fils3(int semId) {
    srand(time(NULL) + getpid());

    sleep(random_(1, 5));

    P(semId, 2);
    P(semId, 0);

    /* Bloc A3 */
    printf("Debut du bloc A3...\n");
    sleep(random_(1, 5));
    printf("Fin du bloc A3.\n");
    /* Fin bloc A3 */

    V(semId, 2);

    sleep(random_(1, 5));

    /* Bloc B3 */
    printf("Debut du bloc B3...\n");
    sleep(random_(1, 5));
    printf("Fin du bloc B3.\n");
    /* Fin bloc B3 */

    sleep(random_(1, 5));

    P(semId, 3);

    /* Bloc C3 */
    printf("Debut du bloc C3...\n");
    sleep(random_(1, 5));
    printf("Fin du bloc C3.\n");
    /* Fin bloc C3 */

    V(semId, 4);

    exit(EXIT_SUCCESS);
}

/**
 * Fonction principale.
 * @param argc le nombre d'arguments
 * @param argv les arguments
 * @return le code de retour
 */
int main(int argc, char *argv[]) {
    int i, n, semId;
    unsigned short init[5] = {0, 1, 1, 0, 0};

    if((semId = semget((key_t) KEY, 5, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if(errno == EEXIST) {
            fprintf(stderr, "Array of semaphores with key %d already exists\n", KEY);
        }

        else {
            perror("An error occurred while attempting to create the array of semaphores ");
        }

        exit(EXIT_FAILURE);
    }

    if(semctl(semId, 0, SETALL, init) == -1) {
        perror("An error occurred while attempting to initialize the semaphores ");
        exit(EXIT_FAILURE);
    }

    /* Creation des fils */
    for(i = 1; i <= 3; i++) {
        n = fork();
        if(n == -1) {
            fprintf(stderr, "Erreur lors de la creation du fils %d", i);
            perror(" ");
            exit(EXIT_FAILURE);
        }
        else if(n == 0) {
            switch(i) {
                case 1: fils1(semId); break;
                case 2: fils2(semId); break;
                case 3: fils3(semId); break;
            }
        }
    }

    /* Attente de la fin des fils */
    for(i = 0; i < 3; i++) {
        if(wait(NULL) == -1) {
            perror("Erreur lors de la fin d'un fils ");
            exit(EXIT_FAILURE);
        }
    }

    if(semctl(semId, 0, IPC_RMID) == -1) {
        perror("An error occurred while attempting to remove the array of semaphores ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}
