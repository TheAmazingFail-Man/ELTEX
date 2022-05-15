#include <stdio.h>

int main(void)
{
    int number = 0xAABBCCDD;
    char *byte = &number, new_byte;
   
    printf("number: %x\n", number);
    printf("1 byte: %hhu\n", *(byte + 3));
    printf("2 byte: %hhu\n", *(byte + 2));
    printf("3 byte: %hhu\n", *(byte + 1));
    printf("4 byte: %hhu\n", *byte);

    printf("Enter new value for 3 byte: ");
    scanf("%hhu", &new_byte);
    printf("%hhu\n", new_byte);

    *(byte + 2) = new_byte;
    printf("new number: %x\n", number);

    return 0;
}
