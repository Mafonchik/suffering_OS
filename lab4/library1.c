#include <math.h>
#include "contract.h"

#ifdef __cplusplus
extern "C" {
#endif

float cos_derivative(float a, float dx) {
    if (dx <= 0.0f) {
        return 0.0f;
    }
    // Реализация №1: f'(x) = (f(a + dx) - f(a))/dx
    return (cosf(a + dx) - cosf(a)) / dx;
}

float pi(int k) {
    if (k <= 0) {
        return 0.0f;
    }

    // Реализация №1: Ряд Лейбница
    double sum = 0.0;
    for (int i = 0; i < k; ++i) {
        double term = 1.0 / (2 * i + 1);
        if (i % 2 == 1) {
            sum -= term;
        } else {
            sum += term;
        }
    }
  
    return (float)(4.0 * sum);
}

#ifdef __cplusplus
}
#endif