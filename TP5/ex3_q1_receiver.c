#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void handler(int sig, siginfo_t *info, void *nothing) {
    int pid = info->si_pid;

    printf("Received signal %d from %d\n", sig, pid);

    if (kill(pid, sig) == -1) {
        fprintf(stderr, "An error occurred while sending the signal the pid %d\n", pid);
        exit(EXIT_FAILURE);
    }

    printf("Sent the same signal back to pid %d\n", pid);
}

int main() {
    sigset_t sigsWait, sigsBlocked;
    siginfo_t info;
    int numSig;

    if (sigaddset(&sigsBlocked, SIGRTMIN) == -1) {
        fprintf(stderr, "An error occurred while adding the signal to the sigset\n");
        exit(EXIT_FAILURE);
    }

    if (sigprocmask(SIG_BLOCK, &sigsBlocked, NULL) == -1) {
        fprintf(stderr, "An error occurred while setting the mask on the sigset\n");
        exit(EXIT_FAILURE);
    }

    if (sigemptyset(&sigsWait) == -1) {
        fprintf(stderr, "An error occurred while emptying the sigset\n");
        exit(EXIT_FAILURE);
    }

    if (sigaddset(&sigsWait, SIGRTMIN) == -1) {
        fprintf(stderr, "An error occurred while adding the signal to the sigset\n");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for signal\n");

    if ((numSig = sigwaitinfo(&sigsWait, &info)) == -1) {
        perror("An error occurred with sigwaitinfo ");
        exit(EXIT_FAILURE);
    }

    handler(numSig, &info, NULL);

    return EXIT_SUCCESS;
}