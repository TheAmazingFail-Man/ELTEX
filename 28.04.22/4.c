#include <stdio.h>

#define SIZE 5                                              //size matrix

int main(void)
{
    int matrix[SIZE][SIZE];
    int north = 0, west = -1, south = SIZE, east = SIZE;    //snail borders
    
    for (int count = 1;;) {
        for (int i = west + 1; i < east; i++) {             //west->east filling
            matrix[north][i] = count++;
        }
        east--;
        for (int i = north + 1; i < south; i++) {           //north->south filling
            matrix[i][east] = count++;
        }
        south--;
        for (int i = east - 1; i > west; i--) {             //east->west filling
            matrix[south][i] = count++;
        }
        west++;
        for (int i = south - 1; i > north; i--) {           //south->north filling
            matrix[i][west] = count++;
        }
        north++;

        if (north >= south && west >= east) {               //checking for exit
            break;
        }
    }

    for (int i = 0; i < SIZE; i++) {                        //print matrix
        for (int j = 0; j < SIZE; j++) {
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }

    return 0;
}
