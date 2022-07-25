#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 256

int main()
{
    int priority = 0;

    mqd_t mqdes_ser_to_cli = mq_open("/task_1_ser_to_cli", O_RDONLY);
    mqd_t mqdes_cli_to_ser = mq_open("/task_1_cli_to_ser", O_WRONLY);

    struct mq_attr *attr = (struct mq_attr *)malloc(sizeof(struct mq_attr));
    mq_getattr(mqdes_ser_to_cli, attr);
    char *buf = (char *)malloc(sizeof(char) * attr->mq_msgsize);

    printf("Waiting message\n");
    mq_receive(mqdes_ser_to_cli, buf, attr->mq_msgsize + 1, &priority);
    printf("Message: %s\n", buf);

    printf("Send message\n");
    sprintf(buf, "Hello server");
    mq_send(mqdes_cli_to_ser, buf, strlen(buf), 0);

    mq_close(mqdes_ser_to_cli);
    mq_close(mqdes_cli_to_ser);
    exit(EXIT_SUCCESS);
}
