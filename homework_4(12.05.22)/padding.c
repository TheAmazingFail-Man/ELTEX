#include <stdio.h>

struct List_1 {
    char a;
    int b;
};

struct List_2 {
    char a;
    int b;
} __attribute__((packed));

int main(void)
{
    struct List_1 list_unpacked = {1, 1};
    struct List_2 list_packed = {1, 1};

    unsigned char *ptr = (char *)&list_unpacked;
    printf("size unpacked = %ld\n", sizeof(struct List_1));
    for (int i = 0; i < 10; i++, ptr++) {
        printf("%d byte\t%hhu\n", i + 1, *ptr);
    }

    ptr = (char *)&list_packed;
    printf("\nsize packed = %ld\n", sizeof(struct List_2));
    for (int i = 0; i < 10; i++, ptr++) {
        printf("%d byte\t%hhu\n", i + 1, *ptr);
    }

    return 0;
}
