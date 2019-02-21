#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    sigset_t sigsWait, sigsBlocked;
    siginfo_t info;
    int pid, value;
    union sigval sigval;

    if (argc == 3) {
        pid = atoi(argv[1]);
        value = atoi(argv[2]);
    }

    else {
        printf("Please enter the PID to send the signal to: ");
        if (scanf("%d", &pid) != 1) {
            fprintf(stderr, "An error occurred while reading input from keyboard\n");
            exit(EXIT_FAILURE);
        }
        printf("\n");

        printf("Please enter the value to send to PID %d: ", pid);
        if (scanf("%d", &value) != 1) {
            fprintf(stderr, "An error occurred while reading input from keyboard\n");
            exit(EXIT_FAILURE);
        }
        printf("\n");
    }

    sigval.sival_int = value;

    /* Send signal to the PID */
    if (sigqueue(pid, SIGRTMIN, sigval) == -1) {
        fprintf(stderr, "An error occurred while sending the value to the pid %d\n", pid);
        perror("");
        exit(EXIT_FAILURE);
    }

    printf("Sent value to pid %d\n", pid);

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

    printf("Received value %d from pid %d\n", info.si_value.sival_int, info.si_pid);

    return EXIT_SUCCESS;
}