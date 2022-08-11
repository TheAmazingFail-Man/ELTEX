#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>

#define BUF_SIZE 256
#define LISTEN_COUNT 5
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 7777
#define NUMBER_CLIENTS 5

int main(int argc, char *argv[])
{
    int tcp_sock = 0,
        udp_sock = 0,
        client_sock = 0,
        client_size = sizeof(struct sockaddr_in),
        snd_bytes = 0,
        rcv_bytes = 0,
        largest_sock = 0,
        ret = 0;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    struct sockaddr_in server, client;
    fd_set fd_in, fd_backup;

    memset(&server, 0, sizeof(struct sockaddr_in));
    memset(&client, 0, sizeof(struct sockaddr_in));
    
    if ((tcp_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket tcp");
        exit(EXIT_FAILURE);
    }

    if ((udp_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket udp");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port = htons(SERVER_PORT);
    if (bind(tcp_sock, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1) {
        perror("bind tcp");
        exit(EXIT_FAILURE);
    }
    if (bind(udp_sock, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1) {
        perror("bind udp");
        exit(EXIT_FAILURE);
    }
    
    if (listen(tcp_sock, LISTEN_COUNT) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&fd_in);
    FD_SET(tcp_sock, &fd_in);
    FD_SET(udp_sock, &fd_in);
    memcpy(&fd_backup, &fd_in, sizeof(fd_set));
    
    largest_sock = tcp_sock > udp_sock ? tcp_sock : udp_sock;


    while (true) {
        memcpy(&fd_in, &fd_backup, sizeof(fd_set));
        ret = select(largest_sock + 1, &fd_in, NULL, NULL, NULL);
        if (ret == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        } else if (ret == 0) {
            printf("timeout\n");
        } else {
            if (FD_ISSET(tcp_sock, &fd_in)) {               //connect udp client
                client_sock = accept(tcp_sock, (struct sockaddr *)&client, &client_size);
                if (client_sock == -1) {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                memcpy(&fd_in, &fd_backup, sizeof(fd_set));
                FD_SET(client_sock, &fd_in);
                memcpy(&fd_backup, &fd_in, sizeof(fd_set));
                largest_sock = largest_sock > client_sock ? largest_sock : client_sock;
                printf("register tcp client\n");
            } else if (FD_ISSET(udp_sock, &fd_in)) {        //recv msg from udp client
                rcv_bytes = recvfrom(udp_sock, buf, BUF_SIZE, 0, (struct sockaddr *)&client, &client_size);
                if (rcv_bytes == -1) {
                    perror("recvfrom");
                    exit(EXIT_FAILURE);
                }
                printf("udp msg: |%s|\n", buf);
            } else if (FD_ISSET(client_sock, &fd_in)) {     //recv msg from tcp client
                rcv_bytes = recv(client_sock, buf, BUF_SIZE, 0);
                if (rcv_bytes == -1) {
                    perror("recv");
                    exit(EXIT_FAILURE);
                }
                printf("tcp msg: |%s|\n", buf);
            }
        }
    }

    exit(EXIT_SUCCESS);
}
