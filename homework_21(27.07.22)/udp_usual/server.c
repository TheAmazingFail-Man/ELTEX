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
#define CLIENT_MAX 10
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 7777

struct Args {
    int sock;
    struct sockaddr_in client;
};

void *client_service(void *args)
{
    struct Args *client_fd = (struct Args *)args;
    int rcv_bytes = 0;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    sprintf(buf, "hello client");
    while (true) {
        rcv_bytes = sendto(client_fd->sock, buf, BUF_SIZE, 0, (struct sockaddr *)&client_fd->client, sizeof(client_fd->client));
        if (rcv_bytes == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[])
{
    int server_fd = 0,
        client_size[CLIENT_MAX],
        client_count = 0,
        status[CLIENT_MAX],
        rcv_bytes = 0,
        snd_bytes = 0;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    memset(buf, 0, sizeof(char) * BUF_SIZE);
    struct sockaddr_in server, client[CLIENT_MAX];
    struct Args client_fd[CLIENT_MAX];
    memset(&server, 0, sizeof(struct sockaddr_in));
    memset(&client, 0, sizeof(struct sockaddr_in) * CLIENT_MAX);
    memset(&client_fd, 0, sizeof(struct Args) * CLIENT_MAX);
    pthread_t tid[CLIENT_MAX];
    for (int i = 0; i < CLIENT_MAX; i++) {
        client_size[i] = sizeof(struct sockaddr_in);
        status[i] = 0;
        tid[i] = 0;
    }

    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
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

    printf("ip: %s\nport: %d\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));
    
    while (client_count != CLIENT_MAX) {
        rcv_bytes = recvfrom(server_fd, buf, BUF_SIZE, 0, (struct sockaddr *)&client[client_count], &client_size[client_count]);
        if (rcv_bytes == -1) {
            perror("1recvfrom");
            exit(EXIT_FAILURE);
        }

        printf("rcv msg: |%s|\n", buf);

        client_fd[client_count].client.sin_family = AF_INET;
        client_fd[client_count].client.sin_addr.s_addr = inet_addr("127.0.0.2");
        client_fd[client_count].client.sin_port = htons(7778);
    
        if (bind(server_fd, (struct sockaddr *)&client_fd[client_count].client, sizeof(struct sockaddr_in)) == -1) {
            perror("1bind");
            exit(EXIT_FAILURE);
        }

        sprintf(buf, "127.0.0.2");
        snd_bytes = sendto(server_fd, buf, strlen(buf), 0, (struct sockaddr *)&client[client_count], client_size[client_count]);
        if (snd_bytes == -1) {
            perror("sendto");
            exit(EXIT_FAILURE);
        }

        client_fd[client_count].sock = server_fd;

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
