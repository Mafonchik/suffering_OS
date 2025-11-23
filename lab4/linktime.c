#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "contract.h"

int main() {
    printf("\nCommands:\n");
    printf("  1 a dx    - compute derivative of cos(x) at point a with increment dx\n");
    printf("  2 k       - compute pi using k terms\n");
    printf("\n");

    char buffer[256];
    while (1) {
        printf("Enter command: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }

        // Удаляем символ новой строки
        buffer[strcspn(buffer, "\n")] = '\0';

        if (buffer[0] == '1') {
            float a, dx;
            if (sscanf(buffer + 1, "%f %f", &a, &dx) == 2) {
                float result = cos_derivative(a, dx);
                printf("Result (cos derivative): %.6f\n", result);
            } else {
                printf("Error: Invalid args for command 1. Expected: 1 a dx\n");
            }
            continue;
        }

        if (buffer[0] == '2') {
            int k;
            if (sscanf(buffer + 1, "%d", &k) == 1) {
                float result = pi(k);
                printf("Result (pi approximation): %.6f\n", result);
            } else {
                printf("Error: Invalid args for command 2. Expected: 2 k\n");
            }
            continue;
        }

        if (buffer[0] == '\0') {
            continue;
        }

        printf("Error: Unknown command. Use 1 or 2.\n");
    }

    return 0;
}