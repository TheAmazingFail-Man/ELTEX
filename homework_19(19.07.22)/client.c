#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <curses.h>
#include <dirent.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <semaphore.h>

#include "data.h"

int shm_ser_to_cli,
    shm_cli_to_ser;
char *chat,
     *nickname,
     *buf;
struct Client_api *api;
sem_t *sem_ser_to_cli,
      *sem_cli_to_ser,
      *sem_sync;

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
    nickname = (char *)malloc(sizeof(char) * NICKNAME_SIZE);

    wprintw(api->cmd_str, "Enter nickname: ");                                          //enter nickname
    wrefresh(api->cmd_str);
    
    wgetnstr(api->cmd_str, nickname, NICKNAME_SIZE);
    if (strlen(nickname) == NICKNAME_SIZE - 1) {                                        //check overflow buf
        while(wgetch(api->cmd_str) != '\n');
    }

    sprintf(buf, "(new user)%s", nickname);

    sem_post(sem_cli_to_ser);
}

void *update_client(void *args)
{
    struct Database *data = (struct Database *)args;

    while(true) {
        sem_wait(sem_ser_to_cli);
        data_output(api, data, chat);
        sem_wait(sem_sync);
    }
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

    shm_ser_to_cli = shm_open("/ser_to_cli", O_RDWR, 0600);
    shm_cli_to_ser = shm_open("/cli_to_ser", O_RDWR, 0600);

    struct Database *data;
    data = mmap(NULL, sizeof(struct Database), PROT_READ | PROT_WRITE, MAP_SHARED, shm_ser_to_cli, 0);
    buf = mmap(NULL, MSG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_cli_to_ser, 0);

    sem_ser_to_cli = sem_open("/ser_to_cli", O_CREAT);
    sem_cli_to_ser = sem_open("/cli_to_ser", O_CREAT);
    sem_sync = sem_open("/sync", O_CREAT);

    chat = (char *)malloc(sizeof(char) * CHAT_SIZE);
    
    login_user(api, data);

    pthread_t tid;
    pthread_create(&tid, NULL, update_client, (void *)data);       //listen server for update

    char *buf_temp = (char *)malloc(sizeof(char) * MSG_SIZE);
    while (true) {
        wclear(api->cmd_str);
        wrefresh(api->cmd_str);
        wprintw(api->cmd_str, "Message: ");
        wgetnstr(api->cmd_str, buf_temp, MSG_SIZE);
        if (strlen(buf_temp) == MSG_SIZE - 1) {
            while(wgetch(api->cmd_str) != '\n');
        }
        sprintf(buf, "(%s): \"%s\"\n", nickname, buf_temp);
        sem_post(sem_cli_to_ser);
    }

    pthread_join(tid, NULL);

    sem_close(sem_cli_to_ser);
    munmap(data, sizeof(struct Database));
    munmap(buf, MSG_SIZE);
    endwin();
    exit(EXIT_SUCCESS);
}
