#include <stdio.h>
#include <stdbool.h>

#define BUF_SIZE 256

void filling_values(double *value_1, double *value_2)
{
    for (bool exit = false; !exit;) {
        printf("Enter 1 value: ");
        scanf("%lf", &(*value_1));
        if (getchar() != '\n')  {
            while (getchar() != '\n');
            printf("ERROR: Invalid value. Please try again\n");
        } else {
            exit = true;
        }
    }
    for (bool exit = false; !exit;) {
        printf("Enter 2 value: ");
        scanf("%lf", &(*value_2));
        if (getchar() != '\n')  {
            while (getchar() != '\n');
            printf("ERROR: Invalid value. Please try again\n");
        } else {
            exit = true;
        }
    }
}

double addition(double value_1, double value_2)
{
    return value_1 + value_2;
}

double subtraction(double value_1, double value_2)
{
    return value_1 - value_2;
}

double multiplication(double value_1, double value_2)
{
    return value_1 * value_2;
}

double division(double value_1, double value_2)
{
    return value_1 / value_2;
}
