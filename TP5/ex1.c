#define _POSIX_SOURCE
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define SECONDS 10

int count = 0;
int flag = 1;

void handler(int sig) {
    switch (sig) {
        case SIGALRM:
            printf("\nHello World\n");
            printf("Count: %d\n", count);
            flag = 0;
            break;

        case SIGINT:
            count++;
            break;
    }
}

int main() {
    struct sigaction action;

    sigemptyset(&action.sa_mask);
    action.sa_handler = handler;

    if (sigaction(SIGALRM, &action, NULL) == -1) {
        fprintf(stderr, "An error occurred while catching the SIGALRM signal\n");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGINT, &action, NULL) == -1) {
        fprintf(stderr, "An error occurred while catching the SIGINT signal\n");
        exit(EXIT_FAILURE);
    }

    printf("Waiting %d seconds\n", SECONDS);
    alarm(SECONDS);

    while (flag == 1) {
        pause();
    }

    return EXIT_SUCCESS;
}