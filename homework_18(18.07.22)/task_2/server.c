#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define BUF_SIZE 64

struct msgbuf {
    long priority;
    char text[BUF_SIZE];
};

int main()
{
    int msqid_ser_to_cli = 0,
        msqid_cli_to_ser = 0;
    struct msgbuf *buf = (struct msgbuf *)malloc(sizeof(struct msgbuf));
    buf->priority = 1;
    sprintf(buf->text, "Hello client");

    msqid_ser_to_cli = msgget(ftok("server", 1), IPC_CREAT | 0600);
    msqid_cli_to_ser = msgget(ftok("client", 2), IPC_CREAT | 0600);

    msgsnd(msqid_ser_to_cli, buf, sizeof(struct msgbuf), 0);
    
    printf("server: ");
    msgrcv(msqid_cli_to_ser, buf, sizeof(struct msgbuf), 0, 0);
    printf("%ld %s\n", buf->priority, buf->text);
    
    msgctl(msqid_ser_to_cli, IPC_RMID, NULL);
    msgctl(msqid_cli_to_ser, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}
