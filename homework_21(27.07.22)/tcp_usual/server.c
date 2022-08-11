#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>

#define BUF_SIZE 256
#define LISTEN_COUNT 5
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 7777

struct Args {
    int sock;
};

void *client_service(void *args)
{
    struct Args *client_fd = (struct Args *)args;
    int rcv_bytes = 0;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    while (true) {
        memset(buf, 0, sizeof(char) * BUF_SIZE);
        rcv_bytes = recv(client_fd->sock, buf, BUF_SIZE, 0);
        if (rcv_bytes == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }
        printf("rcv msg: |%s| rcv bytes: |%d|\n", buf, rcv_bytes);
    }
}

int main(int argc, char *argv[])
{
    int server_fd = 0,
        client_size[LISTEN_COUNT],
        client_count = 0,
        status[LISTEN_COUNT];
    struct sockaddr_in server, client[LISTEN_COUNT];
    struct Args client_fd[LISTEN_COUNT];
    memset(&server, 0, sizeof(struct sockaddr_in));
    memset(&client, 0, sizeof(struct sockaddr_in) * LISTEN_COUNT);
    memset(&client_fd, 0, sizeof(struct Args) * LISTEN_COUNT);
    pthread_t tid[LISTEN_COUNT];
    for (int i = 0; i < LISTEN_COUNT; i++) {
        client_size[i] = sizeof(struct sockaddr_in);
        status[i] = 0;
        tid[i] = 0;
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port = htons(SERVER_PORT);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, LISTEN_COUNT) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("ip: %s\nport: %d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));
    
    while (client_count != LISTEN_COUNT) {
        client_fd[client_count].sock = accept(server_fd, (struct sockaddr *)&client[client_count], &client_size[client_count]);
        if (client_fd[client_count].sock == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        if (pthread_create(&tid[client_count], NULL, client_service, (void *)&client_fd[client_count]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
        client_count++;
    }
    for (int i = 0; i < client_count; i++) {
        status[i] = pthread_join(tid[i], NULL);
    }

    exit(EXIT_SUCCESS);
}
