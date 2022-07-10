#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define SIZE_BUF 256

int main()
{
    char *buf = (char *)malloc(sizeof(char) * SIZE_BUF),
         *command = NULL,
         *token = NULL,
         **argv = (char **)malloc(sizeof(char *) * SIZE_BUF);
    int count_args = 0;

    printf("Enter command: ");
    fgets(buf, SIZE_BUF, stdin);
    if (strlen(buf) == SIZE_BUF - 1) {
        while(getchar() != '\n');
    }
    buf[strlen(buf) - 1] = '\0';

    command = strtok(buf, " ");
    token = strtok(NULL, " ");

    while (token != NULL) {
        argv[count_args++] = token;
        token = strtok(NULL, " ");
    }

    printf("command: %s\n", command);
    printf("args: ");
    for (int i = 0; i < count_args; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");

    execvp(command, argv);

    exit(EXIT_SUCCESS);
}
