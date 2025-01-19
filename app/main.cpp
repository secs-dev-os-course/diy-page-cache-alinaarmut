#include <iostream>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include "include/api.h"
#define BLOCK_SIZE 4096
#define NUMBERS_COUNT (BLOCK_SIZE / 4)
#define NUMBER 1000
#define FILE_SIZE (4 * 1024 * 1024)
#define MAX_NUMBER 999
#define MIN_NUMBER 100

int main() {
    int fd = lab2_open("example.txt");
    if (fd == -1) {
        perror("Ошибка в открытие файла");
        return 1;
    }

    size_t numbers_to_write = FILE_SIZE / 4;
    char *buffer = (char *)malloc(FILE_SIZE);
    if (!buffer) {
        perror("Не удалось выделить память для записи");
        lab2_close(fd);
        return 1;
    }

    srand(time(nullptr));
    char *ptr = buffer;

    for (size_t i = 0; i < numbers_to_write; ++i) {
        int number = MIN_NUMBER + rand() % (MAX_NUMBER - MIN_NUMBER + 1);
        ptr += sprintf(ptr, "%d\n", number);
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < FILE_SIZE; i += BLOCK_SIZE) {
        size_t to_write = (i + BLOCK_SIZE <= FILE_SIZE) ? BLOCK_SIZE : FILE_SIZE - i;
        if (lab2_write(fd, buffer + i, to_write) == -1) {
            perror("Ошибка при записи в файл");
            break;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> write_duration = end - start;
    std::cout << "Время записи данных: " << write_duration.count() << " секунд" << std::endl;

    free(buffer);


    char *read_buffer = (char *)malloc(FILE_SIZE);
    if (!read_buffer) {
        perror("Не удалось выделить память для чтения");
        lab2_close(fd);
        return 1;
    }

    start = std::chrono::high_resolution_clock::now();
    if (lab2_lseek(fd, 0, SEEK_SET) == -1) {
        perror("Ошибка при перемещении указателя файла");
        free(read_buffer);
        lab2_close(fd);
        return 1;
    }

    if (lab2_read(fd, read_buffer, FILE_SIZE) == -1) {
        perror("Ошибка при чтении файла");
        free(read_buffer);
        lab2_close(fd);
        return 1;
    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> read_duration = end - start;
    std::cout << "Время чтения данных: " << read_duration.count() << " секунд" << std::endl;

    free(read_buffer);



    if (lab2_fsync(fd) == -1) {
        perror("Ошибка при синхронизации файла с диском");
        lab2_close(fd);
        return 1;
    }

    if (lab2_close(fd) == -1) {
        perror("Ошибка в закрытие файла");
        return 1;
    }

    return 0;
}
