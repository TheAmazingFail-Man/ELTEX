#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <signal.h>

#define SIZE_BUF 256

int main()
{
    char *buf = (char *)malloc(sizeof(char) * SIZE_BUF),
         *token = NULL,
         **argv = (char **)malloc(sizeof(char *) * SIZE_BUF);
    int count_args = 0,
        fildes[2],
        status = 0;
    pid_t child = 0;

    printf("Enter command: ");                  //enter command
    fgets(buf, SIZE_BUF, stdin);
    if (strlen(buf) == SIZE_BUF - 1) {
        while(getchar() != '\n');
    }
    buf[strlen(buf) - 1] = '\0';

    argv[count_args++] = strtok(buf, " ");      //strtok args
    argv[count_args++] = " ";
    token = strtok(NULL, " ");

    while (token != NULL) {
        argv[count_args++] = token;
        argv[count_args++] = " ";
        token = strtok(NULL, " ");
    }
    count_args--;
    argv[count_args] = 0;

    for (int i = 0; i < count_args; i++) {      //if exist "|"
        if (strcmp(argv[i], "|") == 0) {
            pipe(fildes);
            child = fork();
            if (child) {
                waitpid(child, &status, 0);
                child = fork();
                if (child) {
                    dup2(fildes[1], 1);
                    //write(1, "^Z", 2);                //need fix
                    //fputc((int)'\x1a', stdout);
                    waitpid(child, &status, 0);
                    exit(EXIT_SUCCESS);
                } else {
                    dup2(fildes[0], 0);
                    execvp(argv[i + 2], &argv[i + 3]);
                }
            } else {
                dup2(fildes[1], 1);
                argv[i - 1] = 0;
                execvp(argv[0], &argv[1]);
            }
        }
    }

    execvp(argv[0], &argv[1]);

    exit(EXIT_SUCCESS);
}
