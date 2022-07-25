#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>

#define BUF_SIZE 256

int main()
{
    mkfifo("pipe", 0700);
    int pipe = open("pipe", O_RDONLY);
    char buf[BUF_SIZE] = {};
    read(pipe, &buf, BUF_SIZE);
    printf("%s\n", buf);

    exit(EXIT_SUCCESS);
}
