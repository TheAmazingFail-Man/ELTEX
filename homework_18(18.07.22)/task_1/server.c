#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int priority = 0;

    mq_unlink("/task_1_ser_to_cli");
    mq_unlink("/task_1_cli_to_ser");
    mqd_t mqdes_ser_to_cli = mq_open("/task_1_ser_to_cli", O_WRONLY | O_CREAT, 0600, NULL);
    mqd_t mqdes_cli_to_ser = mq_open("/task_1_cli_to_ser", O_RDONLY | O_CREAT, 0600, NULL);
    
    struct mq_attr *attr = (struct mq_attr *)malloc(sizeof(struct mq_attr));
    mq_getattr(mqdes_ser_to_cli, attr);
    char *buf = (char *)malloc(sizeof(char) * attr->mq_msgsize);

    sprintf(buf, "Hello client");
    printf("Send message\n");
    mq_send(mqdes_ser_to_cli, buf, strlen(buf), 0);

    printf("Waiting message\n");
    memset(buf, 0, sizeof(char) * attr->mq_msgsize);
    mq_receive(mqdes_cli_to_ser, buf, attr->mq_msgsize + 1, &priority);
    printf("Message: %s\n", buf);

    mq_close(mqdes_ser_to_cli);
    mq_close(mqdes_cli_to_ser);
    exit(EXIT_SUCCESS);
}
