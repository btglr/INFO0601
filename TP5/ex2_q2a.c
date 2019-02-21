#define _POSIX_SOURCE
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define SECONDS 5

void handler(int sig) {
    printf("Signal: %d\n", sig);
}

int main() {
    int i, result;
    sigset_t sig_s, sig_blocked;
    struct sigaction action;

    if (sigemptyset(&action.sa_mask) == -1) {
        fprintf(stderr, "An error occurred while emptying the sigset\n");
        exit(EXIT_FAILURE);
    }

    action.sa_handler = handler;

    if (sigfillset(&sig_s) == -1) {
        fprintf(stderr, "An error occurred while filling the sigset\n");
        exit(EXIT_FAILURE);
    }

    if (sigprocmask(SIG_SETMASK, &sig_s, NULL) == -1) {
        fprintf(stderr, "An error occurred while setting the mask on the sigset\n");
        exit(EXIT_FAILURE);
    }

    sleep(SECONDS);

    if (sigpending(&sig_blocked) == -1) {
        fprintf(stderr, "An error occurred while getting the pending signals\n");
        exit(EXIT_FAILURE);
    }

    printf("\n");
    for (i = 1; i <= 32; ++i) {
        if ((result = sigismember(&sig_blocked, i)) == -1) {
            fprintf(stderr, "An error occurred while verifying if signal is a member of the sigset\n");
            exit(EXIT_FAILURE);
        }

        else if (result == 1) {
            printf("Received signal %d\n", i);
        }
    }

    return EXIT_SUCCESS;
}
