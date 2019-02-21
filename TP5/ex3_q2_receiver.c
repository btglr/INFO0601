#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void handler(int sig, siginfo_t *info, void *nothing) {
    int pid = info->si_pid;
    int value = info->si_value.sival_int;
    union sigval sigval;

    printf("Received signal %d from %d\n", sig, pid);
    printf("Received value: %d\n", value);

    sigval.sival_int = value * 2;

    if (sigqueue(pid, sig, sigval) == -1) {
        fprintf(stderr, "An error occurred while killing the pid %d\n", pid);
        perror("");
        exit(EXIT_FAILURE);
    }

    printf("Sent the value %d back to pid %d\n", value * 2, pid);
}

int main() {
    sigset_t sigsWait, sigsBlocked;
    siginfo_t info;
    int numSig;

    if (sigaddset(&sigsBlocked, SIGRTMIN) == -1) {
        perror("An error occurred while adding the signal to the sigset ");
        exit(EXIT_FAILURE);
    }

    if (sigprocmask(SIG_BLOCK, &sigsBlocked, NULL) == -1) {
        perror("An error occurred while setting the mask on the sigset ");
        exit(EXIT_FAILURE);
    }

    if (sigemptyset(&sigsWait) == -1) {
        perror("An error occurred while emptying the sigset ");
        exit(EXIT_FAILURE);
    }

    if (sigaddset(&sigsWait, SIGRTMIN) == -1) {
        perror("An error occurred while adding the signal to the sigset ");
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