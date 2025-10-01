// child.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void reverse_string(char *str) {
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        len--; // не включаем \n в реверс
    }
    for (int i = 0; i < len / 2; i++) {
        char tmp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = tmp;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];
    FILE *fout = fopen(filename, "a"); // append mode
    if (!fout) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char *line = NULL;
    size_t n = 0;
    ssize_t read;

    // Читаем из stdin (куда родитель направил pipe)
    while ((read = getline(&line, &n, stdin)) != -1) {
        // Сохраняем оригинал для вывода в файл и stdout
        char *original = strdup(line);
        reverse_string(line);

        // Вывод в stdout
        fputs(line, stdout);
        fflush(stdout);

        // Запись инвертированной строки в файл
        fputs(line, fout);
        fflush(fout);

        free(original);
    }

    fclose(fout);
    free(line);
    return 0;
}