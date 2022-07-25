#ifndef DATA_H
#define DATA_H

#define MSG_SIZE 256
#define NICKNAME_SIZE 16
#define MAX_USERS 256
#define CHAT_SIZE 1<<20

enum Commands {
    MSG = 1,
    LOGIN,
    UPDATE
};

struct Database {
    int count_users;
    char nicknames[MAX_USERS][NICKNAME_SIZE];
    char last_msg[MSG_SIZE];
};

struct Client_api {
    WINDOW *win;
    WINDOW *chat;
    WINDOW *users;
    WINDOW *cmd_str;
};

#endif
