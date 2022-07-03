#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_COUNT_SUBS 100
#define SIZE_BUF 50

struct List_subs {
    char first_name[SIZE_BUF];
    char last_name[SIZE_BUF];
    int number;
};

void print_api()                                            //output api
{
    printf("Commands:\n\
            \r1 - Show all subs\n\
            \r2 - Add new sub\n\
            \r3 - Search\n\
            \r4 - Exit\n\
            \rEnter number of command: ");
}

void print_subs(struct List_subs *list, int count_subs)     //output subs
{
    system("clear");
    printf("List of subs:\n");
    for (int i = 0; i < count_subs; i++) {
        printf("%d)\tFirst name: %s\n\
                \r\t Last name: %s\n\
                \r\t    Number: %d\n\n\
                \r", i + 1, list[i].first_name, list[i].last_name, list[i].number);
    }
    if (!count_subs) {
        printf("List empty\n\n");
    }
}

void add_sub(struct List_subs *list, int *count_subs)       //adding subs
{
    system("clear");

    printf("Enter first name: ");
    memset(list[*count_subs].first_name, 0, sizeof(char) * SIZE_BUF);
    fgets(list[*count_subs].first_name, SIZE_BUF, stdin);
    list[*count_subs].first_name[strlen(list[*count_subs].first_name) - 1] = '\0';

    printf("Enter last name: ");
    memset(list[*count_subs].last_name, 0, sizeof(char) * SIZE_BUF);
    fgets(list[*count_subs].last_name, SIZE_BUF, stdin);
    list[*count_subs].last_name[strlen(list[*count_subs].last_name) - 1] = '\0';

    printf("Enter number: ");
    scanf("%d", &list[*count_subs].number);
    while (getchar() != '\n');
    (*count_subs)++;
}

void search_sub(struct List_subs *list, int count_subs)
{
    system("clear");
    char buf[SIZE_BUF];

    printf("Enter search word: ");
    memset(buf, 0, sizeof(char) * SIZE_BUF);
    fgets(buf, SIZE_BUF, stdin);
    buf[strlen(buf) - 1] = '\0';

    for (int i = 0; i < count_subs; i++) {
        if (!strcmp(buf, list[i].first_name) ||
            !strcmp(buf, list[i].last_name) ||
            atoi(buf) == list[i].number) {
            printf("%d)\tFirst name: %s\n\
                    \r\t Last name: %s\n\
                    \r\t    Number: %d\n\n\
                    \r", i + 1, list[i].first_name, list[i].last_name, list[i].number);
        }
    }
}

void load_subs(struct List_subs list[], int *count_subs)
{
    FILE *data = fopen("data.txt", "r");
    if (!data) {
        perror("FILE NOT EXIST\n");
        return;
    }
    fread(list, 1, sizeof(struct List_subs) * MAX_COUNT_SUBS, data);
    fclose(data);
    for (int i = 0; i < MAX_COUNT_SUBS; i++) {
        if (list[i].number) {
            (*count_subs)++;
        }
    }
}

void write_subs(struct List_subs list[], int count_subs)
{
    FILE *data = fopen("data.txt", "w");
    fwrite(list, 1, sizeof(struct List_subs) * MAX_COUNT_SUBS, data);
    fclose(data);
}

int main(void)
{
    struct List_subs list[MAX_COUNT_SUBS];
    memset(list, 0, sizeof(struct List_subs) * MAX_COUNT_SUBS);
    int count_subs = 0;
    load_subs(list, &count_subs);
    int command;

    system("clear");
    for (bool exit = false; !exit;) {
        print_api();
        scanf("%d", &command);
        while (getchar() != '\n');
        switch (command) {
        case 1:
            print_subs(list, count_subs);
            break;
        case 2:
            add_sub(list, &count_subs);
            printf("\nCOUNT: %d\n", count_subs);
            break;
        case 3:
            search_sub(list, count_subs);
            break;
        case 4:
            exit = true;
            break;
        default:
            system("clear");
            printf("Wrong command. Try again\n\n");
            break;
        }
    }
    write_subs(list, count_subs);

    return 0;
}
