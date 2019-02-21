#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

#define MAX_LENGTH 1024

int main(int argc, char *argv[]) {
    int key, c, id;
    int *pidAddress;
    char *msgAddress;
    void *address;
    char msg[MAX_LENGTH];

    if (argc == 3) {
        key = atoi(argv[1]);
        memset(msg, '\0', sizeof(msg));
        strncpy(msg, argv[2], MAX_LENGTH - 1);
    }

    else {
        printf("Please enter the key: ");

        if (scanf("%d", &key) != 1) {
            fprintf(stderr, "An error occurred while reading input from keyboard\n");
            exit(EXIT_FAILURE);
        }

        while (((c = getchar()) != '\n') || (c == EOF));
        printf("Please enter the message to store: ");

        if (scanf("%1023[^\n]s", msg) != 1) {
            fprintf(stderr, "An error occurred while reading input from keyboard\n");
            exit(EXIT_FAILURE);
        }
    }

    if ((id = shmget((key_t) key, sizeof(int) + MAX_LENGTH, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
        if (errno != EEXIST) {
            perror("An error occurred while attempting to open the shared memory segment ");
            exit(EXIT_FAILURE);
        }

        /* If we get here then the shared memory segment already exists, so we open it normally and read its content */
        if ((id = shmget((key_t) key, sizeof(int) + MAX_LENGTH, S_IRUSR | S_IWUSR)) == -1) {
            perror("An error occurred while attempting to open the shared memory segment ");
            exit(EXIT_FAILURE);
        }

        if ((address = shmat(id, NULL, 0)) == (void*) -1) {
            perror("An error occurred while attempting to attach to the shared memory segment ");
            exit(EXIT_FAILURE);
        }

        pidAddress = (int*) address;
        msgAddress = (char*) address + sizeof(int);

        printf("PID: %d | Message: %s\n", *pidAddress, msgAddress);

        if (shmdt(address) == -1) {
            perror("An error occurred while attempting to detach the shared memory segment ");
            exit(EXIT_FAILURE);
        }
    }

    if ((address = shmat(id, NULL, 0)) == (void*) -1) {
        perror("An error occured while attempting to attach to the shared memory segment ");
        exit(EXIT_FAILURE);
    }

    pidAddress = (int*) address;
    msgAddress = (char*) address + sizeof(int);

    *pidAddress = getpid();
    strcpy(msgAddress, msg);

    return EXIT_SUCCESS;
}