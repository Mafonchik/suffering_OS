#include "header.h"
#include <sys/wait.h>

int main() {
    char filename1[256], filename2[256];
    printf("Введите название выходного файла для нечётных строк: \n");
    if (!fgets(filename1, sizeof(filename1), stdin)) exit(EXIT_FAILURE);
    filename1[strcspn(filename1, "\n")] = 0;

    printf("Введите название выходного файла для чётных строк: \n");
    if (!fgets(filename2, sizeof(filename2), stdin)) exit(EXIT_FAILURE);
    filename2[strcspn(filename2, "\n")] = 0;

    // Генерируем уникальные имена
    char shm_name1[256], shm_name2[256];
    char sem_empty1[256], sem_full1[256];
    char sem_empty2[256], sem_full2[256];

    generate_name(shm_name1, sizeof(shm_name1), "shm1");
    generate_name(shm_name2, sizeof(shm_name2), "shm2");
    generate_name(sem_empty1, sizeof(sem_empty1), "empty1");
    generate_name(sem_full1, sizeof(sem_full1), "full1");
    generate_name(sem_empty2, sizeof(sem_empty2), "empty2");
    generate_name(sem_full2, sizeof(sem_full2), "full2");

    // Создаём shared memory
    int fd1 = shm_open(shm_name1, O_CREAT | O_RDWR, 0666);
    int fd2 = shm_open(shm_name2, O_CREAT | O_RDWR, 0666);
    if (fd1 == -1 || fd2 == -1) { perror("shm_open"); exit(EXIT_FAILURE); }

    ftruncate(fd1, sizeof(SharedBuffer));
    ftruncate(fd2, sizeof(SharedBuffer));

    SharedBuffer *buf1 = mmap(NULL, sizeof(SharedBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
    SharedBuffer *buf2 = mmap(NULL, sizeof(SharedBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
    if (buf1 == MAP_FAILED || buf2 == MAP_FAILED) { perror("mmap"); exit(EXIT_FAILURE); }

    // Инициализируем буферы
    memset(buf1, 0, sizeof(SharedBuffer));
    memset(buf2, 0, sizeof(SharedBuffer));

    // Создаём семафоры
    sem_t *empty1 = sem_open(sem_empty1, O_CREAT | O_EXCL, 0666, 1); // 1 слот
    sem_t *full1  = sem_open(sem_full1,  O_CREAT | O_EXCL, 0666, 0);
    sem_t *empty2 = sem_open(sem_empty2, O_CREAT | O_EXCL, 0666, 1);
    sem_t *full2  = sem_open(sem_full2,  O_CREAT | O_EXCL, 0666, 0);

    if (empty1 == SEM_FAILED || full1 == SEM_FAILED ||
        empty2 == SEM_FAILED || full2 == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Запуск потомков
    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Child 1
        execl("./child", "child", filename1, shm_name1, sem_empty1, sem_full1, (char*)NULL);
        perror("execl child1");
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Child 2
        execl("./child", "child", filename2, shm_name2, sem_empty2, sem_full2, (char*)NULL);
        perror("execl child2");
        exit(EXIT_FAILURE);
    }

    // Родитель: записывает в буферы
    char *line = NULL;
    size_t n = 0;
    ssize_t read;
    int line_number = 0;

    while ((read = getline(&line, &n, stdin)) != -1) {
        line_number++;

        if (line_number % 2 == 1) {
            // Нечётная → buf1
            sem_wait(empty1);
            if (read <= MAX_LINE) {
                memcpy(buf1->data, line, read);
                buf1->size = read;
                buf1->ready = 1;
            }
            sem_post(full1);
        } else {
            // Чётная → buf2
            sem_wait(empty2);
            if (read <= MAX_LINE) {
                memcpy(buf2->data, line, read);
                buf2->size = read;
                buf2->ready = 1;
            }
            sem_post(full2);
        }
    }

    // Посылаем сигнал завершения: отправляем пустую строку с ready=0
    sem_wait(empty1);
    buf1->ready = 0;
    sem_post(full1);

    sem_wait(empty2);
    buf2->ready = 0;
    sem_post(full2);

    // Ожидаем завершения
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    // Очистка
    munmap(buf1, sizeof(SharedBuffer));
    munmap(buf2, sizeof(SharedBuffer));
    close(fd1);
    close(fd2);

    shm_unlink(shm_name1);
    shm_unlink(shm_name2);

    sem_close(empty1); sem_close(full1);
    sem_close(empty2); sem_close(full2);
    sem_unlink(sem_empty1); sem_unlink(sem_full1);
    sem_unlink(sem_empty2); sem_unlink(sem_full2);


    free(line);
    return 0;
}
