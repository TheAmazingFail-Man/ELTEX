#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define BUF_SIZE 256
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 7777

int main(int argc, char *argv[])
{
    int server_fd = 0,
        client_fd = 0,
        client_size = sizeof(struct sockaddr_in),
        snd_bytes = 0,
        rcv_bytes = 0;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    struct sockaddr_in server, client;
    memset(&server, 0, sizeof(struct sockaddr_in));
    memset(&client, 0, sizeof(struct sockaddr_in));

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port = htons(SERVER_PORT);

    if (connect(server_fd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    sprintf(buf, "hello server");
    snd_bytes = send(server_fd, buf, strlen(buf), 0);
    if (snd_bytes == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }
    rcv_bytes = recv(server_fd, buf, BUF_SIZE, 0);
    if (rcv_bytes == -1) {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    printf("rcv msg: |%s| rcv bytes: |%d|\n", buf, rcv_bytes);

    exit(EXIT_SUCCESS);
}
