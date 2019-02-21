#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int key, height, width, x, y, value, shmId, i;
    int *arrAddress, *dataAddress;
    void *address;

    if (argc == 4) {
        key = atoi(argv[1]);
        width = atoi(argv[2]);
        height = atoi(argv[3]);

        if ((shmId = shmget((key_t) key, 2 * sizeof(int) + sizeof(int) * width * height, S_IRUSR | S_IWUSR | IPC_CREAT | IPC_EXCL)) == -1) {
            if (errno == EEXIST) {
                fprintf(stderr, "Shared memory segment with key %d already exists\n", key);
                perror("");
                exit(EXIT_FAILURE);
            }

            else {
                perror("An error occurred while attempting to open the shared memory segment ");
                exit(EXIT_FAILURE);
            }
        }

        if ((address = shmat(shmId, NULL, 0)) == (void*) -1) {
            perror("An error occurred while attempting to attach to the shared memory segment ");
            exit(EXIT_FAILURE);
        }

        dataAddress = (int*) address;
        dataAddress[0] = width;
        dataAddress[1] = height;

        arrAddress = (int*) address + 2 * sizeof(int);

        for (i = 0; i < width * height; ++i) {
            arrAddress[i] = 0;
        }
    }

    else if (argc == 5) {
        key = atoi(argv[1]);
        x = atoi(argv[2]);
        y = atoi(argv[3]);
        value = atoi(argv[4]);

        if ((shmId = shmget((key_t) key, 0, 0)) == -1) {
            if (errno == ENOENT) {
                fprintf(stderr, "Shared memory segment with key %d doesn't exist\n", key);
                perror("");
                exit(EXIT_FAILURE);
            }

            else {
                perror("An error occurred while attempting to open the shared memory segment ");
                exit(EXIT_FAILURE);
            }
        }

        if ((address = shmat(shmId, NULL, 0)) == (void*) -1) {
            perror("An error occurred while attempting to attach to the shared memory segment ");
            exit(EXIT_FAILURE);
        }

        dataAddress = (int*) address;
        width = dataAddress[0];
        height = dataAddress[1];

        arrAddress = (int*) address + 2 * sizeof(int);

        arrAddress[width * y + x] = value;

        for (i = 0; i < width * height; ++i) {
            printf("%d ", arrAddress[i]);

            if ((i + 1) % width == 0) {
                printf("\n");
            }
        }
    }

    else {
        fprintf(stderr, "Not enough or too many arguments were provided\n");
        exit(EXIT_FAILURE);
    }

    if (shmdt(address) == -1) {
        perror("An error occurred while attempting to detach the shared memory segment ");
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;
}