#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define BUF_SIZE 256
#define PORT 7777

int main(int argc, char *argv[])
{
    int sock_fd = 0,
        server_size = sizeof(struct sockaddr_in),
        snd_bytes = 0,
        rcv_bytes = 0;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    struct sockaddr_in server;
    memset(&server, 0, sizeof(struct sockaddr_in));

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(PORT);

    for (;;) {
        sprintf(buf, "hello server");
        snd_bytes = sendto(sock_fd, buf, strlen(buf), 0, (struct sockaddr *)&server, sizeof(server));
        if (snd_bytes == -1) {
            perror("sendto");
            exit(EXIT_FAILURE);
        }
        
        rcv_bytes = recvfrom(sock_fd, buf, BUF_SIZE, 0, (struct sockaddr *)&server, &server_size);
        if (rcv_bytes == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        
        printf("msg: |");
        for (int i = 0; i < rcv_bytes; i++) {
            printf("%c|", buf[i]);
        }
        printf("\n");
    }

    exit(EXIT_SUCCESS);
}
