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
#define SERVER_PORT 7777

int main(int argc, char *argv[])
{
    int sock_fd = 0,
        client_size = sizeof(struct sockaddr_in),
        rcv_bytes = 0;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    struct sockaddr_in server, client;
    memset(&server, 0, sizeof(struct sockaddr_in));
    memset(&client, 0, sizeof(struct sockaddr_in));

    if ((sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(SERVER_PORT);

    printf("ip: |%s|\nport: |%d|\n", inet_ntoa(server.sin_addr), ntohs(server.sin_port));

    for (;;) {
        rcv_bytes = recvfrom(sock_fd, buf, BUF_SIZE, 0, (struct sockaddr *)&client, &client_size);
        if (rcv_bytes == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        //printf("rcv msg: |%s| rcv bytes: |%d|\n", buf, rcv_bytes);
        printf("msg:|");
        for (int i = 8; i < rcv_bytes; i++) {
            printf("%c|", buf[i]);
        }
        printf("\n");
    }

    exit(EXIT_SUCCESS);
}
