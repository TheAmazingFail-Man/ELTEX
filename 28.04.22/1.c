#include <stdio.h>

#define SIZE 5                                  //size matrix

int main(void)
{
    int matrix[SIZE][SIZE];

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            matrix[i][j] = i * SIZE + j + 1;
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }

    return 0;
}
