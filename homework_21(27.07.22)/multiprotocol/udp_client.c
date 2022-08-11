#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>

#define BUF_SIZE 256
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 7777

int main(int argc, char *argv[])
{
    int server_fd = 0,
        server_size = sizeof(struct sockaddr_in),
        snd_bytes = 0,
        rcv_bytes = 0;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    struct sockaddr_in server;
    memset(&server, 0, sizeof(struct sockaddr_in));

    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port = htons(SERVER_PORT);

    sprintf(buf, "%s", argv[1]);
    while (true) {
        snd_bytes = sendto(server_fd, buf, strlen(buf), 0, (struct sockaddr *)&server, sizeof(server));
        if (snd_bytes == -1) {
            perror("sendto");
            exit(EXIT_FAILURE);
        }
        printf("sendto msg");
        sleep(1);
    }

    exit(EXIT_SUCCESS);
}
