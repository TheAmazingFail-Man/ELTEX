#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define BUF_SIZE 256
#define ADDRESS "/tcp_local"
#define LISTEN_COUNT 5

int main(int argc, char *argv[])
{
    int server_fd = 0,
        client_fd = 0,
        client_size = sizeof(struct sockaddr_un),
        snd_bytes = 0,
        rcv_bytes = 0;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    struct sockaddr_un server, client;
    memset(&server, 0, sizeof(struct sockaddr_un));
    memset(&client, 0, sizeof(struct sockaddr_un));

    if (unlink(ADDRESS) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    if ((server_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server.sun_family = AF_LOCAL;
    strncpy(server.sun_path, ADDRESS, sizeof(server.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, LISTEN_COUNT) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    client_fd = accept(server_fd, (struct sockaddr *)&client, &client_size);
    if (client_fd == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    rcv_bytes = recv(client_fd, buf, BUF_SIZE, 0);
    if (rcv_bytes == -1) {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    printf("rcv msg: |%s| rcv bytes: |%d|\n", buf, rcv_bytes);
    sprintf(buf, "hello client");
    snd_bytes = send(client_fd, buf, strlen(buf), 0);
    if (snd_bytes == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
