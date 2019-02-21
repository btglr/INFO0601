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
    sigset_t sig_s, sig_blocked;
    struct sigaction action;

    sigemptyset(&action.sa_mask);
    action.sa_handler = handler;

    sigfillset(&sig_s);
    sigprocmask(SIG_BLOCK, &sig_s, NULL);

    sleep(SECONDS);

    sigpending(&sig_blocked);

    sigaction(SIGINT, &action, NULL);

    return EXIT_SUCCESS;
}
