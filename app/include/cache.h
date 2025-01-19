#ifndef CACHE_H
#define CACHE_H
#include <unistd.h>

#define BLOCK_SIZE 4096
#define MAX_BLOCKS 4

typedef struct CacheBlock {
    off_t offset;
    void *data;
    size_t size;
    int dirty;
    struct CacheBlock *next;
} CacheBlock;


typedef struct FileCache {
    int fd;
    CacheBlock *head;
    size_t block_size;
    size_t max_blocks;
    size_t current_blocks;
} FileCache;


int cache_free(FileCache *cache);
CacheBlock *cache_find(FileCache *cache, off_t offset);
int cache_add_block(FileCache *cache, off_t offset, const void *data, size_t size);
int linux_random_evict(FileCache *cache);
int cache_initialize(FileCache *cache, int fd);

#endif
