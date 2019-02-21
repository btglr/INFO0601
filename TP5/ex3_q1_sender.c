#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    sigset_t sigsWait, sigsBlocked;
    siginfo_t info;
    int pid;

    if (argc == 2) {
        pid = atoi(argv[1]);
    }

    else {
        printf("Please enter the PID to send the signal to: ");
        if (scanf("%d", &pid) != 1) {
            fprintf(stderr, "An error occurred while reading input from keyboard\n");
            exit(EXIT_FAILURE);
        }
        printf("\n");
    }

    /* Send signal to the PID */
    if (kill(pid, SIGRTMIN) == -1) {
        fprintf(stderr, "An error occurred while sending the signal the pid %d\n", pid);
        perror("");
        exit(EXIT_FAILURE);
    }

    printf("Sent signal to pid %d\n", pid);

    /* Block the signal that we'll receive as a response (otherwise: "Real-time signal" message -> process exits) */
    if (sigaddset(&sigsBlocked, SIGRTMIN) == -1) {
        perror("An error occurred while adding the signal to the sigset ");
        exit(EXIT_FAILURE);
    }

    if (sigprocmask(SIG_BLOCK, &sigsBlocked, NULL) == -1) {
        perror("An error occurred while setting the mask on the sigset ");
        exit(EXIT_FAILURE);
    }

    /* Add the signal we want to wait for to another set */
    if (sigemptyset(&sigsWait) == -1) {
        perror("An error occurred while emptying the sigset ");
        exit(EXIT_FAILURE);
    }

    if (sigaddset(&sigsWait, SIGRTMIN) == -1) {
        perror("An error occurred while adding the signal to the sigset ");
        exit(EXIT_FAILURE);
    }

    /* Wait for the signal */
    if (sigwaitinfo(&sigsWait, &info) == -1) {
        perror("An error occurred with sigwaitinfo ");
        exit(EXIT_FAILURE);
    }

    printf("Received signal back from pid %d\n", info.si_pid);

    return EXIT_SUCCESS;
}