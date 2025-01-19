#ifndef API_H
#define API_H
#include <stdio.h>


int lab2_open(const char *path);
int lab2_close(int fd);
ssize_t lab2_read(int fd, void *buf, size_t count);
ssize_t lab2_write(int fd, const void *buf, size_t count);
off_t lab2_lseek(int fd, off_t offset, int whence); // Перестановка позиции указателя на данные файла
int lab2_fsync(int fd);


#endif
