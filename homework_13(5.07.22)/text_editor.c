#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdlib.h>
#include <curses.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#define SIZE_TEXT 1<<20

struct Text_editor {
    WINDOW *win;
    WINDOW *subwin_text;
    WINDOW *subwin_help;
    FILE *file;
    char *text;
    int pos_cur;
    int count_sym;
} *te;

void text_output(struct Text_editor *te)
{
    int y = 0, x = 0;
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);

    getyx(te->subwin_text, y, x);
    wclear(te->subwin_text);
    for (int i = 0; i < te->count_sym; i++) {
        wprintw(te->subwin_text, "%c", te->text[i]);
    }

    wmove(te->subwin_text, y, x);
    wrefresh(te->subwin_text);
}

void win_output(struct Text_editor *te)
{
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);

    te->win = newwin(size.ws_row, size.ws_col, 0, 0);
    wbkgd(te->win, COLOR_PAIR(1));

    int size_other_data = (size.ws_col / 2 - 2) / 10 * 2;

    te->subwin_text = derwin(te->win, size.ws_row - 1, size.ws_col, 0, 0);
    te->subwin_help = derwin(te->win, 1, size.ws_col, size.ws_row - 1, 0);
    wprintw(te->subwin_help, "F1 - save and quit\tF2 - quit without save");

    wrefresh(te->win);
    wrefresh(te->subwin_text);
    wrefresh(te->subwin_help);
}

void sig_winch(int signo)
{
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);
    resizeterm(size.ws_row, size.ws_col);
    refresh();

    win_output(te);
    text_output(te);
}

void key_up(struct Text_editor *te)
{
    int y = 0, x = 0, offset = 0, offset_temp = 0, length_step = 1;
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);

    getyx(te->subwin_text, y, x);
    while (te->pos_cur > 0 && te->text[te->pos_cur - 1] != '\n') {
        te->pos_cur--;
        offset++;
    }
    if (te->pos_cur != 0) {
        te->pos_cur--;
        for (int i = te->pos_cur - 1; i >= 0 && te->text[i] != '\n'; i--) {
            offset_temp++;
        }
        for (int i = 0; i < offset_temp - offset; i++) {
            te->pos_cur--;
        }
        if (offset >= size.ws_col) {
            length_step += offset / size.ws_col;
        }
        if (offset_temp / size.ws_col > offset / size.ws_col) {
            length_step += offset_temp / size.ws_col - offset / size.ws_col;
        }
        offset = offset < offset_temp ? offset : offset_temp;
        while (offset >= size.ws_col) {
            offset -= size.ws_col;
        }
        wmove(te->subwin_text, y - length_step, offset);
    } else {
        wmove(te->subwin_text, 0, 0);
    }
}

void key_down(struct Text_editor *te)
{
    int y = 0, x = 0, offset = 0, offset_temp = 0, length_step = 1;
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);

    getyx(te->subwin_text, y, x);
    for (int i = te->pos_cur - 1; i >= 0 && te->text[i] != '\n'; i--) {
        offset++;
    }
    while (te->pos_cur < te->count_sym && te->text[te->pos_cur] != '\n') {
        te->pos_cur++;
        offset_temp++;
    }
    if (te->text[te->pos_cur] == '\n' || offset + offset_temp >= size.ws_col) {
        if (offset + offset_temp >= size.ws_col) {
            length_step += (offset + offset_temp) / size.ws_col;
        }
        te->pos_cur++;
        offset_temp = 0;
        for (int i = 0; te->pos_cur < te->count_sym && te->text[te->pos_cur] != '\n' && i < offset; i++) {
            te->pos_cur++;
            offset_temp++;
        }
        if (offset / size.ws_col > offset_temp / size.ws_col) {
            length_step -= offset / size.ws_col - offset_temp / size.ws_col;
        }
        offset = offset == offset_temp ? offset : offset_temp;
        while (offset >= size.ws_col) {
            offset -= size.ws_col;
        }
        wmove(te->subwin_text, y + length_step, offset);
    } else if (te->pos_cur == te->count_sym) {
        wmove(te->subwin_text, y, offset + offset_temp);
    }
}

void key_left(struct Text_editor *te)
{
    int y = 0, x = 0, offset = 0;
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);

    getyx(te->subwin_text, y, x);
    if (te->pos_cur > 0) {
        if (te->text[te->pos_cur - 1] == '\n' || x == 0) {
            te->pos_cur--;
            offset = 0;
            for (int i = te->pos_cur - 1; i >= 0 && te->text[i] != '\n'; i--) {
                offset++;
            }
            while (offset >= size.ws_col) {
                offset -= size.ws_col;
            }
            wmove(te->subwin_text, y - 1, offset);
        } else {
            te->pos_cur--;
            wmove(te->subwin_text, y, x - 1);
        }
    }
}

void key_right(struct Text_editor *te)
{
    int y = 0, x = 0, offset = 0;
    struct winsize size;
    ioctl(fileno(stdout), TIOCGWINSZ, (char *)&size);

    getyx(te->subwin_text, y, x);
    if (te->text[te->pos_cur] == '\n' || x == size.ws_col - 1) {
        te->pos_cur++;
        wmove(te->subwin_text, y + 1, 0);
    } else if (te->pos_cur < te->count_sym) {
        te->pos_cur++;
        wmove(te->subwin_text, y, x + 1);
    }
}

void key_backspace(struct Text_editor *te)
{
    if (te->count_sym) {
        key_left(te);
        memcpy(&te->text[te->pos_cur], &te->text[te->pos_cur + 1], te->count_sym - te->pos_cur + 1);
        te->count_sym--;
        te->text[te->count_sym] = '\0';
        text_output(te);
    }
}

void key_add(struct Text_editor *te, int ch)
{
    te->count_sym++;
    for (int i = te->count_sym; i >= te->pos_cur; i--) {
        te->text[i] = te->text[i - 1];
    }
    te->text[te->pos_cur] = ch;
    key_right(te);
    text_output(te);
}

void handler_keys(struct Text_editor *te, char *file)
{
    int ch = 0;
    bool exit = false;

    while(!exit) {
        ch = getch();
        switch (ch) {
        case KEY_UP:
            key_up(te);
            break;
        case KEY_DOWN:
            key_down(te);
            break;
        case KEY_LEFT:
            key_left(te);
            break;
        case KEY_RIGHT:
            key_right(te);
            break;
        case KEY_BACKSPACE:
            key_backspace(te);
            break;
        case KEY_F(1):
            te->file = fopen(file, "w");
            for (int i = 0; i < te->count_sym; i++) {
                fputc((int)te->text[i], te->file);
            }
            fclose(te->file);
            exit = true;
            break;
        case KEY_F(2):
            exit = true;
            break;
        default:
            key_add(te, ch);
            break;
        }
        wrefresh(te->subwin_text);
    }
}

int main(int argc, char *argv[])
{
    initscr();

    signal(SIGWINCH, sig_winch);
    cbreak();
    curs_set(true);
    keypad(stdscr, true);
    start_color();
    refresh();

    init_pair(1, COLOR_WHITE, COLOR_BLACK);

    te = (struct Text_editor *)malloc(sizeof(struct Text_editor));
    te->text = (char *)malloc(sizeof(char) * SIZE_TEXT);
    te->count_sym = 0;
    te->pos_cur = 0;

    te->file = fopen(argv[1], "r");
    for (char sym = fgetc(te->file); !feof(te->file); sym = fgetc(te->file), te->count_sym++) {
        te->text[te->count_sym] = sym;
    }
    fclose(te->file);

    win_output(te);
    text_output(te);

    handler_keys(te, argv[1]);
    
    endwin();
    exit(EXIT_SUCCESS);
}
