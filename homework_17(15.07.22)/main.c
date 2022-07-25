#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

int main()
{
    int signal = 0;
    pid_t child = 0;

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &set, 0);

    child = fork();
    if (child) {
        while (true) {
            sleep(1);
            kill(child, SIGUSR1);
        }
    } else {
        while (true) {
            sigwait(&set, &signal);
            if (signal == SIGUSR1) {
                printf("Signal detected\n");
            }
        }
    }

    exit(EXIT_SUCCESS);
}
