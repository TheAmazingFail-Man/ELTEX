#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <curses.h>
#include <dirent.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "data.h"

char *chat,
     *nickname;
mqd_t mqdes_ser_to_cli,
      mqdes_cli_to_ser;
struct Client_api *api;

void api_create(struct Client_api *api)
{
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);

    api->win = newwin(size.ws_row, size.ws_col, 0, 0);
    wbkgd(api->win, COLOR_PAIR(1));
    api->chat = derwin(api->win, size.ws_row - 1, size.ws_col - (size.ws_col / 5), 0, 0);
    api->users = derwin(api->win, size.ws_row - 1, size.ws_col / 5, 0, size.ws_col - (size.ws_col / 5));
    api->cmd_str = derwin(api->win, 1, size.ws_col, size.ws_row - 1, 0);

    wrefresh(api->win);
    wrefresh(api->chat);
    wrefresh(api->users);
    wrefresh(api->cmd_str);
}

void data_output(struct Client_api *api, struct Database *data, char *chat)
{
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
    sprintf(chat, "%s%s", chat, data->last_msg);
    wclear(api->chat);
    wprintw(api->chat, "%s", chat);
    wclear(api->users);
    for (int i = 0; i < data->count_users; i++) {
        wprintw(api->users, "%s\n", data->nicknames[i]);
    }

    wrefresh(api->chat);
    wrefresh(api->users);
}

void login_user(struct Client_api *api, struct Database *data)
{
    int priority = 0;
    nickname = (char *)malloc(sizeof(char) * NICKNAME_SIZE);

    wprintw(api->cmd_str, "Enter nickname: ");                                          //enter nickname
    wrefresh(api->cmd_str);
    
    wgetnstr(api->cmd_str, nickname, NICKNAME_SIZE);
    if (strlen(nickname) == NICKNAME_SIZE - 1) {                                        //check overflow buf
        while(wgetch(api->cmd_str) != '\n');
    }
    mq_send(mqdes_cli_to_ser, nickname, NICKNAME_SIZE, LOGIN);                          //send nickname
}

void *update_client(void *args)
{
    int priority = 0;
    struct Database *data = (struct Database *)args;
    mqdes_ser_to_cli = mq_open("/ser_to_cli", O_RDONLY);
    struct mq_attr *attr = (struct mq_attr *)malloc(sizeof(struct mq_attr));
    mq_getattr(mqdes_ser_to_cli, attr);

    chat = (char *)malloc(sizeof(char) * CHAT_SIZE);
    while(true) {
        mq_receive(mqdes_ser_to_cli, (char *)data, attr->mq_msgsize + 1, &priority);
        switch(priority) {
        case UPDATE:
            data_output(api, data, chat);
            break;
        }
    }
    mq_close(mqdes_ser_to_cli);
}

int main()
{
    initscr();
    //signal(SIGWINCH, sig_winch);
    curs_set(true);
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    api = (struct Client_api *)malloc(sizeof(struct Client_api));
    api_create(api);

    mqdes_cli_to_ser = mq_open("/cli_to_ser", O_WRONLY);

    struct Database *data = (struct Database *)malloc(sizeof(struct Database));
    login_user(api, data);
    
    pthread_t tid;
    pthread_create(&tid, NULL, update_client, (void *)data);       //listen server for update

    char *buf = (char *)malloc(sizeof(char) * MSG_SIZE);
    char *buf_temp = (char *)malloc(sizeof(char) * MSG_SIZE);
    while (true) {
        wclear(api->cmd_str);
        wrefresh(api->cmd_str);
        wprintw(api->cmd_str, "Message: ");
        wgetnstr(api->cmd_str, buf, MSG_SIZE);
        if (strlen(buf) == MSG_SIZE - 1) {
            while(wgetch(api->cmd_str) != '\n');
        }
        sprintf(buf_temp, "(%s): \"%s\"\n", nickname, buf);
        mq_send(mqdes_cli_to_ser, buf_temp, MSG_SIZE, MSG);
    }

    pthread_join(tid, NULL);

    mq_close(mqdes_cli_to_ser);
    endwin();
    exit(EXIT_SUCCESS);
}
