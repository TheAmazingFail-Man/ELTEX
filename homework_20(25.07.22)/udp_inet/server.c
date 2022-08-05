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

#define BUF_SIZE 256
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 7777

int main(int argc, char *argv[])
{
    int server_fd = 0,
        client_size = sizeof(struct sockaddr_in),
        snd_bytes = 0,
        rcv_bytes = 0;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    struct sockaddr_in server, client;
    memset(&server, 0, sizeof(struct sockaddr_in));
    memset(&client, 0, sizeof(struct sockaddr_in));

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

    rcv_bytes = recvfrom(server_fd, buf, BUF_SIZE, 0, (struct sockaddr *)&client, &client_size);
    if (rcv_bytes == -1) {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    printf("rcv msg: |%s| rcv bytes: |%d|\n", buf, rcv_bytes);
    printf("address: |%s| size: |%d|\n", inet_ntoa(client.sin_addr), client_size);
    sprintf(buf, "hello client");
    snd_bytes = sendto(server_fd, buf, strlen(buf), 0, (struct sockaddr *)&client, client_size);
    if (snd_bytes == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
