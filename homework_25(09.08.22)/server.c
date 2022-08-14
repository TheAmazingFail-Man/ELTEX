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
#include <stdint.h>

#define BUF_SIZE 256
#define PORT 7777
#define MSG "Hello client"

struct Udp_header {
    uint16_t source_port;
    uint16_t dest_port;
    uint16_t length;
    uint16_t checksum;
};

struct Ip_header {
    uint8_t version_ihl;
    uint8_t ds;
    uint16_t length;
    uint16_t id;
    uint16_t flag_offset;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint32_t sourse_ip;
    uint32_t dest_ip;
};

int main(int argc, char *argv[])
{
    int sock_fd = 0,
        client_size = sizeof(struct sockaddr_in),
        snd_bytes = 0,
        rcv_bytes = 0,
        msg_length = 0;
    const int optval = 1;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    struct sockaddr_in server, client;
    struct Udp_header udp_header;
    struct Ip_header ip_header;
    memset(&server, 0, sizeof(struct sockaddr_in));
    memset(&client, 0, sizeof(struct sockaddr_in));
    memset(&udp_header, 0, sizeof(struct Udp_header));
    memset(&ip_header, 0, sizeof(struct Ip_header));

    if ((sock_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sock_fd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) == -1) {
        perror("setsockopt IP_HDRINCL");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    udp_header.source_port = htons(PORT);
    udp_header.dest_port = 0;
    udp_header.length = htons(sizeof(struct Udp_header) + strlen(MSG));
    udp_header.checksum = 0;

    ip_header.version_ihl = 69;
    ip_header.ds = 0;
    ip_header.length = 0;
    ip_header.id = 0;
    ip_header.flag_offset = 64;
    ip_header.ttl = 255;
    ip_header.protocol = IPPROTO_UDP;
    ip_header.checksum = 0;
    ip_header.sourse_ip = 0;
    ip_header.dest_ip = inet_addr("127.0.0.1");
    
    for (;;) {
        rcv_bytes = recvfrom(sock_fd, buf, BUF_SIZE, 0, (struct sockaddr *)&client, &client_size);
        if (rcv_bytes == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        
        printf("rcv msg:|");
        for (int i = 0; i < rcv_bytes; i++) {
            printf("%c|", buf[i]);
        }
        printf("\n");
        
        if (udp_header.dest_port == 0) {
            memcpy(&udp_header.dest_port, &buf[20], 2);
        }
        memset(buf, 0, sizeof(char) * BUF_SIZE);
        memcpy(buf, &ip_header, sizeof(struct Ip_header));
        memcpy(&buf[sizeof(struct Ip_header)], &udp_header, sizeof(struct Udp_header));
        memcpy(&buf[sizeof(struct Ip_header) + sizeof(struct Udp_header)], MSG, strlen(MSG));
        msg_length = sizeof(struct Ip_header) + sizeof(struct Udp_header) + strlen(MSG);
    
        /*printf("snd buf:|");
        for (int i = 0; i < sizeof(struct Ip_header) + sizeof(struct Udp_header) + strlen(MSG); i++) {
            printf("%c|", buf[i]);
        }
        printf("\n");*/

        snd_bytes = sendto(sock_fd, buf, msg_length, 0, (struct sockaddr *)&client, client_size);
        if (snd_bytes == -1) {
            perror("sendto");
            exit(EXIT_FAILURE);
        }
        sleep(1);
    }

    exit(EXIT_SUCCESS);
}
