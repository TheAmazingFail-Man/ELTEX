#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdbool.h>

#define BUF_SIZE 256
#define SERVER_PORT 7777

int main(int argc, char *argv[])
{
    int sock_fd = 0,
        server_size = sizeof(struct sockaddr_in),
        rcv_bytes = 0;
    const int optval = 1;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    struct sockaddr_in server;
    struct ip_mreq mreq;
    memset(&server, 0, sizeof(struct sockaddr_in));
    memset(&mreq, 0, sizeof(struct ip_mreq));

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        perror("setsockopt so_reuseaddr");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(SERVER_PORT);

    if (bind(sock_fd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    mreq.imr_multiaddr.s_addr = inet_addr(argv[1]);
    //mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    mreq.imr_interface.s_addr = inet_addr(argv[2]);
    if (setsockopt(sock_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) == -1) {
        perror("setsockopt ip_add_membership");
        exit(EXIT_FAILURE);
    }

    while (true) {
        rcv_bytes = recvfrom(sock_fd, buf, BUF_SIZE, 0, (struct sockaddr *)&server, &server_size);
        if (rcv_bytes == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }
        printf("rcv msg: |%s| rcv bytes: |%d|\n", buf, rcv_bytes);
    }

    exit(EXIT_SUCCESS);
}
