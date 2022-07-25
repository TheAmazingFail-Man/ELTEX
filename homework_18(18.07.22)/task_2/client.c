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

    msqid_ser_to_cli = msgget(ftok("server", 1), 0);
    msqid_cli_to_ser = msgget(ftok("client", 2), 0);

    printf("client: ");
    msgrcv(msqid_ser_to_cli, buf, sizeof(struct msgbuf), 0, 0);
    printf("%ld %s\n", buf->priority, buf->text);
    sprintf(buf->text, "Hello server");
    msgsnd(msqid_cli_to_ser, buf, sizeof(struct msgbuf), 0);


    msgctl(msqid_ser_to_cli, IPC_RMID, NULL);
    msgctl(msqid_cli_to_ser, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}
