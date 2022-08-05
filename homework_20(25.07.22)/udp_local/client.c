#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define BUF_SIZE 256
#define ADDRESS "/udp_local"

int main(int argc, char *argv[])
{
    int server_fd = 0,
        server_size = sizeof(struct sockaddr_un),
        snd_bytes = 0,
        rcv_bytes = 0;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    struct sockaddr_un server;
    memset(&server, 0, sizeof(struct sockaddr_un));

    server_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server.sun_family = AF_LOCAL;
    strncpy(server.sun_path, ADDRESS, sizeof(server.sun_path) - 1);

    sprintf(buf, "hello server");
    snd_bytes = sendto(server_fd, buf, strlen(buf), 0, (struct sockaddr *)&server, sizeof(struct sockaddr_un));
    if (snd_bytes == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }
    rcv_bytes = recvfrom(server_fd, buf, BUF_SIZE, 0, (struct sockaddr *)&server, &server_size);
    if (rcv_bytes == -1) {
        perror("recv");
        exit(EXIT_FAILURE);
    }
    printf("rcv msg: |%s| rcv bytes: |%d|\n", buf, rcv_bytes);
    printf("address: |%s| size: |%d|", server.sun_path, server_size);

    exit(EXIT_SUCCESS);
}
