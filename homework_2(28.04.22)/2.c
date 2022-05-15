#include <stdio.h>

#define SIZE 10                             //size array

int main(void)
{
    int array[SIZE], temp;

    printf("array: ");
    for (int i = 0; i < SIZE; i++) {        //filling array
        array[i] = i + 1;
        printf("%d ", array[i]);
    }
    printf("\n");

    for (int i = 0; i < SIZE / 2; i++) {    //reverse array
        temp = array[i];
        array[i] = array[SIZE - 1 - i];
        array[SIZE - 1 - i] = temp;
    }

    printf("reverse array: ");
    for (int i = 0; i < SIZE; i++) {        //print reverse array
        printf("%d ", array[i]);
    }
    printf("\n");

    return 0;
}
