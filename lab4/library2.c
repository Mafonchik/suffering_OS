#include <math.h>
#include "contract.h"

#ifdef __cplusplus
extern "C" {
#endif

float cos_derivative(float a, float dx) {
    if (dx <= 0.0f) {
        return 0.0f;
    }
    // Реализация №2: f'(x) = (f(a + dx) - f(a - dx))/(2dx)
    return (cosf(a + dx) - cosf(a - dx)) / (2 * dx);
}

float pi(int k) {
    if (k <= 0) {
        return 0.0f;
    }

    // Реализация №2: Формула Валлиса
    double product = 1.0;
    for (int n = 1; n <= k; ++n) {
        double num = 2.0 * n;
        double term = (num / (num - 1)) * (num / (num + 1));
        product *= term;
    }

    return (float)(2.0 * product);
}

#ifdef __cplusplus
}
#endif