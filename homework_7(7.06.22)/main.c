#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define SIZE_FILE 6

int main()
{
    char buf[1];
    int file = open("file.txt", O_WRONLY | O_TRUNC | O_CREAT, S_IREAD | S_IWRITE);
    if (file == -1) {
        perror("Failed create file\n");
        exit(-1);
    }
    write(file, "abcdef", SIZE_FILE);

    close(file);

    file = open("file.txt", O_RDONLY);
    if (file == -1) {
        perror("Failed open file for reading\n");
        exit(-1);
    }

    lseek(file, -1, SEEK_END);
    for (int i = 0; i < SIZE_FILE; i++) {
        read(file, buf, 1);
        printf("%c", buf[0]);
        lseek(file, -2, SEEK_CUR);
    }
    printf("\n");

    close(file);

    return 0;
}
