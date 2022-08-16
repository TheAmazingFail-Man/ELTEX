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
#include <linux/if_ether.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <stdbool.h>

#define BUF_SIZE 256
#define PORT 7777
#define INTERFACE_NAME "enp0s8"
#define MSG "hello client"

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

struct Mac_header {
    uint8_t dest_mac[ETH_ALEN];
    uint8_t sourse_mac[ETH_ALEN];
    uint16_t ethertype;
};

int calc_checksum(struct Ip_header ip_header) {
    int checksum = 0,
        tmp = 0;
    uint16_t *ptr = (uint16_t *)&ip_header;

    for (int i = 0; i < 10; i++) {
        checksum += *ptr;
        ptr++;
    }

    tmp = checksum >> 16;
    checksum += tmp;
    checksum = ~checksum;

    return checksum;
}

int main(int argc, char *argv[])
{
    int sock_fd = 0,
        client_size = sizeof(struct sockaddr_ll),
        snd_bytes = 0,
        rcv_bytes = 0,
        msg_length = 0;
    char *buf = (char *)malloc(sizeof(char) * BUF_SIZE);
    struct sockaddr_ll server, client;
    struct Udp_header udp_header;
    struct Ip_header ip_header;
    struct Mac_header mac_header;
    struct ifreq ifr_ip, ifr_mac;

    memset(&server, 0, sizeof(struct sockaddr_ll));
    memset(&client, 0, sizeof(struct sockaddr_ll));
    memset(&udp_header, 0, sizeof(struct Udp_header));
    memset(&ip_header, 0, sizeof(struct Ip_header));
    memset(&mac_header, 0, sizeof(struct Mac_header));
    memset(&ifr_ip, 0, sizeof(struct ifreq));
    memset(&ifr_mac, 0, sizeof(struct ifreq));

    if ((sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memcpy(ifr_ip.ifr_name, INTERFACE_NAME, strlen(INTERFACE_NAME));
    memcpy(ifr_mac.ifr_name, INTERFACE_NAME, strlen(INTERFACE_NAME));
    if (ioctl(sock_fd, SIOCGIFADDR, &ifr_ip) == -1) {           //get ip address
        perror("ioctl SIOCGIFADDR");
        exit(EXIT_FAILURE);
    }
    if (ioctl(sock_fd, SIOCGIFHWADDR, &ifr_mac) == -1) {        //get mac address
        perror("ioctl SIOCGIFHWADDR");
        exit(EXIT_FAILURE);
    }

    printf("IP ADDRESS: |%s|\n", inet_ntoa(((struct sockaddr_in *)&(ifr_ip.ifr_addr))->sin_addr));

    server.sll_family = AF_PACKET;
    server.sll_ifindex = if_nametoindex(INTERFACE_NAME);
    server.sll_halen = ETH_ALEN;
    memcpy(server.sll_addr, ifr_mac.ifr_hwaddr.sa_data, ETH_ALEN);

    printf("MAC ADDRESS: |");
    for (int i = 0; i < ETH_ALEN; i++) {
        printf("%02x:", server.sll_addr[i]);
    }
    printf("|\n");

    udp_header.source_port = htons(PORT);
    udp_header.dest_port = 0;                                   //fillin later
    udp_header.length = htons(sizeof(struct Udp_header) + strlen(MSG));
    udp_header.checksum = 0;

    ip_header.version_ihl = 69;
    ip_header.ds = 0;
    ip_header.length = htons(sizeof(struct Ip_header) + sizeof(struct Udp_header) + strlen(MSG));
    ip_header.id = 0;
    ip_header.flag_offset = 64;
    ip_header.ttl = 255;
    ip_header.protocol = IPPROTO_UDP;
    ip_header.sourse_ip = ((struct sockaddr_in *)&(ifr_ip.ifr_addr))->sin_addr.s_addr;
    ip_header.dest_ip = 0;                                      //fillin later
    ip_header.checksum = 0;                                     //fillin later
    
    memcpy(mac_header.sourse_mac, ifr_mac.ifr_hwaddr.sa_data, ETH_ALEN);
    mac_header.ethertype = htons(0x0800);

    for (bool first = true;;) {
        rcv_bytes = recvfrom(sock_fd, buf, BUF_SIZE, 0, (struct sockaddr *)&client, &client_size);
        if (rcv_bytes == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        
        printf("rcv msg:|");
        for (int i = 0; i < rcv_bytes; i++) {
            if (buf[i] == 'h') {
                //printf("{%d}", i);
            }
            printf("%c|", buf[i]);
        }
        printf("\n");
        
        if (first) {                                            //first recv msg
            memcpy(&udp_header.dest_port, &buf[34], sizeof(uint16_t));
            memcpy(&ip_header.dest_ip, &buf[26], sizeof(uint32_t));
            ip_header.checksum = calc_checksum(ip_header);
            memcpy(mac_header.dest_mac, &buf[6], ETH_ALEN);

            first = false;
        }

        memset(buf, 0, sizeof(char) * BUF_SIZE);
        memcpy(buf, &mac_header, sizeof(struct Mac_header));
        memcpy(&buf[sizeof(struct Mac_header)], &ip_header, sizeof(struct Ip_header));
        memcpy(&buf[sizeof(struct Mac_header) + sizeof(struct Ip_header)], &udp_header, sizeof(struct Udp_header));
        memcpy(&buf[sizeof(struct Mac_header) + sizeof(struct Ip_header) + sizeof(struct Udp_header)], MSG, strlen(MSG));
        msg_length = sizeof(struct Mac_header) + sizeof(struct Ip_header) + sizeof(struct Udp_header) + strlen(MSG);

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
