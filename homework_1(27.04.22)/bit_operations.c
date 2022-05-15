#include <stdio.h>

int main(void)
{
    int a = 0xAABBCCDD;
    unsigned char c;
    printf("number: %x\n", a);
    printf("4 byte: %x\n", (unsigned char)(a >> 24));
    printf("3 byte: %x\n", (unsigned char)((a << 8) >> 24));
    printf("2 byte: %x\n", (unsigned char)((a & 0x0000FF00) >> 8));
    printf("1 byte: %x\n", (unsigned char)(a & 0xFF));

    printf("\nEnter new value for 3 byte: ");
    scanf("%hhu", &c);
    printf("\nnew byte: %x\n", c);
    a = (a & 0xFF00FFFF) | (int)(c << 16);
    printf("new number: %x\n", a);

    return 0;
}
