#include "include/api.h"

#include <cstdio>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "include/cache.h"


FileCache *global_cache = NULL;

int lab2_open(const char *path) {
    int fd = open(path, O_RDWR | O_DIRECT, 0666);
    if (fd == -1) {
        perror("Ошибка открытия файла");
        return -1;
    }

    global_cache = (FileCache *)malloc(sizeof(FileCache));
    if (!global_cache) {
        close(fd);
        return -1;
    }

    if (cache_initialize(global_cache, fd) == -1) {
        free(global_cache);
        close(fd);
        return -1;
    }
    printf("Файл открыт и кэш инициализирован.\n");
    return fd;
}

int lab2_close(int fd) {
    printf("Закрытие файла...\n");

    if (global_cache) {
        cache_free(global_cache);
        free(global_cache);
        global_cache = NULL;
    }

    if (close(fd) == -1) {
        perror("Ошибка при закрытии файла");
        return -1;
    }
    return 0;
}

ssize_t lab2_read(int fd, void *buf, size_t count) {
    if (!global_cache) {
        perror("Кэш не инициализирован");
        return -1;
    }

    off_t offset = lseek(fd, 0, SEEK_CUR);
    CacheBlock *block = cache_find(global_cache, offset);

    if (block) {
        printf("Чтение из кэша...\n");
        size_t to_copy = (count < block->size) ? count : block->size;
        memcpy(buf, block->data, to_copy);
        return to_copy;
    }

    printf("Чтение с диска...\n");
    ssize_t bytes_read = pread(fd, buf, count, offset);
    if (bytes_read == -1) {
        perror("Ошибка при чтении файла");
        return -1;
    }

    if (cache_add_block(global_cache, offset, buf, bytes_read) == -1) {
        perror("Ошибка добавления блока в кэш");
    }

    return bytes_read;
}

off_t lab2_lseek(int fd, off_t offset, int whence) {
    off_t new_offset = lseek(fd, offset, whence);
    if (new_offset == -1) {
        perror("Ошибка перемещения указателя файла");
        return -1;
    }
    return new_offset;
}

int lab2_fsync(int fd) {
    if (!global_cache) {
        perror("Кэш не инициализирован");
        return -1;
    }

    printf("Синхронизация данных кэша с диском...\n");
    CacheBlock *block = global_cache->head;
    while (block) {
        if (block->dirty) {
            printf("Сохранение блока с offset %ld на диск...\n", block->offset);
            if (pwrite(fd, block->data, block->size, block->offset) == -1) {
                perror("Ошибка записи блока на диск");
                return -1;
            }
            block->dirty = 0;
        }
        block = block->next;
    }

    if (fsync(fd) == -1) {
        perror("Ошибка при синхронизации файла с диском");
        return -1;
    }
    return 0;
}
ssize_t lab2_write(int fd, const void *buf, size_t count) {
    if (!global_cache) {
        perror("Кэш не инициализирован");
        return -1;
    }

    off_t offset = lseek(fd, 0, SEEK_CUR);

    CacheBlock *block = cache_find(global_cache, offset);
    if (block) {
        size_t to_copy = (count < global_cache->block_size) ? count : global_cache->block_size;
        memcpy(block->data, buf, to_copy);
        block->dirty = 1;
    } else {
        if (cache_add_block(global_cache, offset, buf, count) == -1) {
            perror("Ошибка добавления блока в кэш");
            return -1;
        }
    }

    return count;
}