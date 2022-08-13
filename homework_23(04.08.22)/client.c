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
#define SERVER_PORT 7777

int main(int argc, char *argv[])
{
    int sock_fd = 0,
        server_size = sizeof(struct sockaddr_in),
        snd_bytes = 0;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    struct sockaddr_in server;
    memset(&server, 0, sizeof(struct sockaddr_in));

    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(SERVER_PORT);

    sprintf(buf, "hello server");
    for (;;) {
        snd_bytes = sendto(sock_fd, buf, strlen(buf), 0, (struct sockaddr *)&server, sizeof(server));
        if (snd_bytes == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }
        sleep(1);
    }

    exit(EXIT_SUCCESS);
}
