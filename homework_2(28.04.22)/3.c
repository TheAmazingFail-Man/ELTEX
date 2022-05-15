#include <stdio.h>

#define SIZE 4                                                  //size matrix

int main(void)
{
    int matrix[SIZE][SIZE];

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            matrix[i][j] = (i + 1) + (j + 1) <= SIZE ? 0 : 1;   //checking diogonal
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }

    return 0;
}
