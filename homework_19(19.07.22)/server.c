#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <curses.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>
#include <semaphore.h>

#include "data.h"

int shm_ser_to_cli,
    shm_cli_to_ser;
char *chat,
     *buf;
bool update;
sem_t *sem_ser_to_cli,
      *sem_cli_to_ser,
      *sem_sync;

void add_user(struct Database *data, char *nickname)                    //add nickname to data
{
    memcpy(nickname, &nickname[10], strlen(nickname) - strlen("(new user)"));
    nickname[strlen(nickname) - strlen("(new user)")] = '\0';
    sprintf(data->nicknames[data->count_users], "%s", nickname);
    printf("LOGIN USER: |%s|\n", data->nicknames[data->count_users]);
    data->count_users++;
    data->last_msg[0] = '\0';
}

void add_message(struct Database *data, char *chat, char *message)      //add msg to chat
{
    sprintf(data->last_msg, "%s", message);
    sprintf(chat, "%s%s", chat, message);
}

void *service_clients(void *args)                                       //receive msgs from clients
{
    int sem_value = 0;
    struct Database *data = (struct Database *)args;

    shm_unlink("/cli_to_ser");
    shm_cli_to_ser = shm_open("/cli_to_ser", O_RDWR | O_CREAT, 0600);
    perror("open shm cli to ser");
    ftruncate(shm_cli_to_ser, MSG_SIZE);
    buf = mmap(NULL, MSG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_cli_to_ser, 0);
    perror("mmap buf");

    sem_unlink("/cli_to_ser");
    sem_cli_to_ser = sem_open("/cli_to_ser", O_CREAT, 0600, 0);
    perror("open sem cli to ser");

    sem_unlink("/sync");
    sem_sync = sem_open("/sync", O_CREAT, 0600, 0);
    perror("open sem sync");

    while (true) {
        sem_wait(sem_cli_to_ser);
        if (memcmp(buf, "(new user)", strlen("(new user)")) == 0) {
            printf("LOGIN NEW USER\n");
            add_user(data, buf);
        } else {
            printf("RCV MSG\n");
            add_message(data, chat, buf);
        }
        for (int i = 0; i < data->count_users; i++) {
            sem_post(sem_ser_to_cli);
        }
        while (true) {
            sem_getvalue(sem_ser_to_cli, &sem_value);
            if (0 == sem_value) {
                break;
            }
        }
        for (int i = 0; i < data->count_users; i++) {
            sem_post(sem_sync);
        }
    }

    sem_close(sem_cli_to_ser);
    munmap(buf, MSG_SIZE);
    return 0;
}

int main()
{
    chat = (char *)malloc(sizeof(char) * CHAT_SIZE);
    memset(chat, 0, sizeof(char) * CHAT_SIZE);
    
    shm_unlink("/ser_to_cli");
    shm_ser_to_cli = shm_open("/ser_to_cli", O_RDWR | O_CREAT, 0600);
    perror("open shm ser to cli");
    ftruncate(shm_ser_to_cli, sizeof(struct Database));
    struct Database *data;
    data = mmap(NULL, sizeof(struct Database), PROT_READ | PROT_WRITE, MAP_SHARED, shm_ser_to_cli, 0);
    perror("mmap data");
    data->count_users = 0;
    
    pthread_t tid;                                                      //launch service clients
    pthread_create(&tid, NULL, service_clients, (void *)data);

    sem_unlink("/ser_to_cli");
    sem_ser_to_cli = sem_open("/ser_to_cli", O_CREAT, 0600, 0);
    perror("open sem ser to cli");

    pthread_join(tid, NULL);

    sem_close(sem_ser_to_cli);
    munmap(data, sizeof(struct Database));
    exit(EXIT_SUCCESS);
}
