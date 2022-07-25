#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int child, fildes[2];
    pipe(fildes);
    child = fork();
    if (child) {
        write(fildes[1], "Hello there", 11);
    } else {
        char buf[11];
        read(fildes[0], buf, 11);
        printf("%s\n", buf);
        printf("Press any key to continue");
        getchar();
    }

    exit(EXIT_SUCCESS);
}
