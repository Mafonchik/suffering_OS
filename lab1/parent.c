// parent.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_LINE 4096

int main() {
    char filename1[256], filename2[256];
    printf("Введите название выходного файла для нечетных строк: \n");
    // Ввод имён файлов
    if (!fgets(filename1, sizeof(filename1), stdin)) {
        fprintf(stderr, "Error reading filename1\n");
        exit(EXIT_FAILURE);
    }
    filename1[strcspn(filename1, "\n")] = 0; // убираем \n
    
    printf("Введите название выходного файла для нечетных строк: \n");
    if (!fgets(filename2, sizeof(filename2), stdin)) {
        fprintf(stderr, "Error reading filename2\n");
        exit(EXIT_FAILURE);
    }
    filename2[strcspn(filename2, "\n")] = 0;

    int pipe1[2], pipe2[2];
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Child 1
    pid_t pid1 = fork();
    if (pid1 == 0) {
        close(pipe1[1]); // закрываем write-конец
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);
        close(pipe2[0]);
        close(pipe2[1]);
        execl("./child", "child", filename1, (char *)NULL);
        perror("execl child1");
        exit(EXIT_FAILURE);
    }

    // Child 2
    pid_t pid2 = fork();
    if (pid2 == 0) {
        close(pipe2[1]);
        dup2(pipe2[0], STDIN_FILENO);
        close(pipe2[0]);
        close(pipe1[0]);
        close(pipe1[1]);
        execl("./child", "child", filename2, (char *)NULL);
        perror("execl child2");
        exit(EXIT_FAILURE);
    }

    // Родитель: закрываем read-концы
    close(pipe1[0]);
    close(pipe2[0]);

    char *line = NULL;
    size_t n = 0;
    ssize_t read;
    int line_number = 0;

    // Читаем строки от пользователя (начиная с 3-й введённой строки!)
    while ((read = getline(&line, &n, stdin)) != -1) {
        line_number++;

        if (line_number % 2 == 1) {
            // Нечётная → pipe1
            write(pipe1[1], line, read);
        } else {
            // Чётная → pipe2
            write(pipe2[1], line, read);
        }
    }

    // Закрываем write-концы → дети получат EOF
    close(pipe1[1]);
    close(pipe2[1]);

    // Ждём завершения детей
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    free(line);
    return 0;
}