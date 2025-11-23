#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

// Прототипы функций
typedef float (*CosDerivativeFunc)(float, float);
typedef float (*PiFunc)(int);

// Заглушки для функций
static float stub_cos_derivative(float a, float dx) {
    (void)a; (void)dx;
    return 0.0f;
}

static float stub_pi(int k) {
    (void)k;
    return 0.0f;
}

int main() {
    void* library = NULL;
    CosDerivativeFunc f_cos_derivative = stub_cos_derivative;
    PiFunc f_pi = stub_pi;

    printf("Enter path to shared library (e.g., ./lib1.so): ");
    char lib_path[256];
    if (fgets(lib_path, sizeof(lib_path), stdin) == NULL) {
        printf("Error reading library path\n");
        return 1;
    }
    
    lib_path[strcspn(lib_path, "\n")] = '\0';

    // Загрузка библиотеки
    library = dlopen(lib_path, RTLD_LAZY);
    if (library == NULL) {
        printf("Error: Failed to load library (%s). Using stubs.\n", dlerror());
    } else {
        f_cos_derivative = (CosDerivativeFunc)dlsym(library, "cos_derivative");
        f_pi = (PiFunc)dlsym(library, "pi");

        if (f_cos_derivative == NULL) {
            printf("Error: 'cos_derivative' not found. Using stub.\n");
            f_cos_derivative = stub_cos_derivative;
        }
        if (f_pi == NULL) {
            printf("Error: 'pi' not found. Using stub.\n");
            f_pi = stub_pi;
        }
    }

    printf("\nCommands:\n");
    printf("  0             - switch to another library\n");
    printf("  1 a dx        - compute derivative of cos(x) at point a with increment dx\n");
    printf("  2 k           - compute pi using k terms\n");
    printf("\n");

    char buffer[256];
    while (1) {
        printf("Enter command: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        if (buffer[0] == '0') {
            if (library) {
                dlclose(library);
                library = NULL;
            }

            printf("Enter new library path: ");
            if (fgets(lib_path, sizeof(lib_path), stdin) == NULL) {
                break;
            }
            lib_path[strcspn(lib_path, "\n")] = '\0';

            library = dlopen(lib_path, RTLD_LAZY);
            if (library == NULL) {
                printf("Error: Failed to load new library (%s). Using stubs.\n", dlerror());
                f_cos_derivative = stub_cos_derivative;
                f_pi = stub_pi;
            } else {
                f_cos_derivative = (CosDerivativeFunc)dlsym(library, "cos_derivative");
                f_pi = (PiFunc)dlsym(library, "pi");

                if (f_cos_derivative == NULL) {
                    printf("Error: 'cos_derivative' not found. Using stub.\n");
                    f_cos_derivative = stub_cos_derivative;
                }
                if (f_pi == NULL) {
                    printf("Error: 'pi' not found. Using stub.\n");
                    f_pi = stub_pi;
                }
                printf("Library switched successfully!\n");
            }
            continue;
        }

        if (buffer[0] == '1') {
            float a, dx;
            if (sscanf(buffer + 1, "%f %f", &a, &dx) == 2) {
                float result = f_cos_derivative(a, dx);
                printf("Result (cos derivative): %.6f\n", result);
            } else {
                printf("Error: Invalid args for command 1. Expected: 1 a dx\n");
            }
            continue;
        }

        if (buffer[0] == '2') {
            int k;
            if (sscanf(buffer + 1, "%d", &k) == 1) {
                float result = f_pi(k);
                printf("Result (pi approximation): %.6f\n", result);
            } else {
                printf("Error: Invalid args for command 2. Expected: 2 k\n");
            }
            continue;
        }

        if (buffer[0] == '\0') {
            continue;
        }

        printf("Error: Unknown command. Use 0, 1, or 2.\n");
    }

    if (library) {
        dlclose(library);
    }

    return 0;
}