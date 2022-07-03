#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "calc.h"

void calc_menu()
{
    printf("Commands:\n\
            \r1 - addition\n\
            \r2 - subtraction\n\
            \r3 - multiplication\n\
            \r4 - division\n\
            \r5 - exit\n\
            \rEnter number of command: ");
}

int main(void)
{
    int command;
    double value_1 = 0, value_2 = 0, answer = 0;

    for (bool exit = false;;) {
        calc_menu();
        scanf("%d", &command);
        if (getchar() != '\n') {
            while (getchar() != '\n');
            command = -1;
        }
        system("clear");
        if (1 <= command && 4 >= command) {
            filling_values(&value_1, &value_2);
        }
        switch (command) {
        case 1:
            answer = addition(value_1, value_2);
            break;
        case 2:
            answer = subtraction(value_1, value_2);
            break;
        case 3:
            answer = multiplication(value_1, value_2);
            break;
        case 4:
            answer = division(value_1, value_2);
            break;
         case 5:
            exit = true;
            break;
        default:
            printf("ERROR: Invalid command entered\n\n");
            break;
        }
        if (exit) {
            break;
        }
        if (1 <= command && 4 >= command) {
            printf("Answer = %lf\n\n", answer);
        }
    }

    return 0;
}
