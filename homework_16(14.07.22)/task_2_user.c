#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main()
{
    int pipe = open("pipe", O_WRONLY);
    write(pipe, "Hello there", 11);

    exit(EXIT_SUCCESS);
}
