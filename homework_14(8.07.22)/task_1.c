#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int status = 0;
    fork();

    printf("pid = %d\tppid = %d\n", getpid(), getppid());
    wait3(&status, WNOHANG, NULL);
    sleep(0.1);

    exit(EXIT_SUCCESS);
}
