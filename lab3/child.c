#include "header.h"

void reverse_string(char *str, size_t len) {
    if (len == 0) return;
    if (str[len - 1] == '\n') len--;
    for (size_t i = 0; i < len / 2; i++) {
        char tmp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = tmp;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <output_file> <shm_name> <sem_empty> <sem_full>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];
    const char *shm_name = argv[2];
    const char *sem_empty_name = argv[3];
    const char *sem_full_name = argv[4];

    FILE *fout = fopen(filename, "a");
    if (!fout) { perror("fopen"); exit(EXIT_FAILURE); }

    // Подключаемся к shared memory
    int fd = shm_open(shm_name, O_RDWR, 0666);
    if (fd == -1) { perror("shm_open child"); exit(EXIT_FAILURE); }

    SharedBuffer *buf = mmap(NULL, sizeof(SharedBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buf == MAP_FAILED) { perror("mmap child"); exit(EXIT_FAILURE); }

    // Подключаемся к семафорам
    sem_t *empty = sem_open(sem_empty_name, 0);
    sem_t *full  = sem_open(sem_full_name, 0);
    if (empty == SEM_FAILED || full == SEM_FAILED) {
        perror("sem_open child");
        exit(EXIT_FAILURE);
    }

    // Цикл обработки
    while (1) {
        sem_wait(full); // Ждём, пока данные появятся

        if (!buf->ready) {
            // Сигнал завершения
            break;
        }

        // Копируем данные (чтобы не менять оригинал в буфере)
        char line_copy[MAX_LINE];
        size_t size = buf->size;
        if (size > MAX_LINE - 1) size = MAX_LINE - 1;
        memcpy(line_copy, buf->data, size);
        line_copy[size] = '\0';

        // Реверс
        reverse_string(line_copy, size);

        // Выводим
        fputs(line_copy, stdout);
        fflush(stdout);

        // Пишем в файл
        fputs(line_copy, fout);
        fflush(fout);

        sem_post(empty); // Освобождаем слот
    }

    fclose(fout);
    munmap(buf, sizeof(SharedBuffer));
    close(fd);
    sem_close(empty);
    sem_close(full);

    return 0;
}
