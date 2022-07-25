#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <curses.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>

#include "data.h"

bool update;
char *chat;
mqd_t mqdes_ser_to_cli,
      mqdes_cli_to_ser;

void add_user(struct Database *data, char *nickname)                    //add nickname to data
{
    sprintf(data->nicknames[data->count_users], "%s", nickname);
    printf("LOGIN USER: |%s|\n", data->nicknames[data->count_users]);
    data->count_users++;
    data->last_msg[0] = '\0';
}

void sig_update(int signo)
{
    update = true;
}

void add_message(struct Database *data, char *chat, char *message)      //add msg to chat
{
    sprintf(data->last_msg, "%s", message);
    sprintf(chat, "%s%s", chat, message);
}

void *service_clients(void *args)                                       //receive msgs from clients
{
    struct Database *data = (struct Database *)args;
    int priority = 0;

    mq_unlink("/cli_to_ser");
    mqdes_cli_to_ser = mq_open("/cli_to_ser", O_RDONLY | O_CREAT, 0600, NULL);
    perror("open cli to ser");
    struct mq_attr *attr = (struct mq_attr *)malloc(sizeof(struct mq_attr));
    mq_getattr(mqdes_cli_to_ser, attr);
    char *buf = (char *)malloc(sizeof(char) * attr->mq_msgsize + 1);

    while (true) {
        mq_receive(mqdes_cli_to_ser, buf, attr->mq_msgsize + 1, &priority);
        switch (priority) {
        case MSG:
            printf("RCV MSG\n");
            add_message(data, chat, buf);
            raise(SIGUSR1);
            break;
        case LOGIN:
            printf("LOGIN NEW USER\n");
            add_user(data, buf);
            raise(SIGUSR1);
            break;
        }
    }

    mq_close(mqdes_cli_to_ser);
    return 0;
}

int main()
{
    struct Database *data = (struct Database *)malloc(sizeof(struct Database));
    data->count_users = 0;
    chat = (char *)malloc(sizeof(char) * CHAT_SIZE);
    memset(chat, 0, sizeof(char) * CHAT_SIZE);
    
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    signal(SIGUSR1, sig_update);

    pthread_t tid;                                                      //launch service clients
    pthread_create(&tid, NULL, service_clients, (void *)data);

    mq_unlink("/ser_to_cli");
    mqdes_ser_to_cli = mq_open("/ser_to_cli", O_WRONLY | O_CREAT, 0600, NULL);
    perror("open ser to cli");

    update = false;
    while (true) {
        if (update) {
            printf("UPDATE DATA\n");
            for (int i = 0; i < data->count_users; i++) {
                mq_send(mqdes_ser_to_cli, (char *)data, sizeof(struct Database), UPDATE);
            }
            update = false;
        }
    }

    mq_close(mqdes_ser_to_cli);
    exit(EXIT_SUCCESS);
}
