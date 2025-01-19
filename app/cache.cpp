#include "include/cache.h"


#include <cstdio>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int cache_free(FileCache *cache) {
    printf("Освобождение кэша...\n");
    CacheBlock *block = cache->head;
    while (block != NULL) {
        CacheBlock *next = block->next;
        free(block->data);
        free(block);
        block = next;
    }
    cache->head = NULL;
    cache->current_blocks = 0;
    return 0;
}


CacheBlock *cache_find(FileCache *cache, off_t offset) {
    CacheBlock *block = cache->head;
    while (block != NULL) {
        if (block->offset == offset) {
            return block;
        }
        block = block->next;
    }
    return NULL;
}


int cache_add_block(FileCache *cache, off_t offset, const void *data, size_t size) {
    if (cache->current_blocks >= cache->max_blocks) {
        if (linux_random_evict(cache) == -1) {
            return -1;
        }
    }

    CacheBlock *new_block = (CacheBlock *)malloc(sizeof(CacheBlock));
    if (new_block == NULL) {
        return -1;
    }

    new_block->data = malloc(size);
    if (new_block->data == NULL) {
        free(new_block);
        return -1;
    }

    memcpy(new_block->data, data, size);
    new_block->offset = offset;
    new_block->size = size;
    new_block->dirty = 0;
    new_block->next = cache->head;
    cache->head = new_block;
    cache->current_blocks++;
    return 0;
}


int linux_random_evict(FileCache *cache) {
    if (cache->head == NULL) {
        return -1;
    }

    srand(time(NULL));

    int evict_index = rand() % cache->current_blocks;
    CacheBlock *prev = NULL;
    CacheBlock *block = cache->head;

    for (int i = 0; i < evict_index; ++i) {
        prev = block;
        block = block->next;
    }


    if (prev == NULL) {
        cache->head = block->next;
    } else {
        prev->next = block->next;
    }

    free(block->data);
    free(block);
    cache->current_blocks--;

    return 0;
}

int cache_initialize(FileCache *cache, int fd) {
    cache->fd = fd;
    cache->head = NULL;
    cache->block_size = BLOCK_SIZE;
    cache->max_blocks = MAX_BLOCKS;
    cache->current_blocks = 0;
    return 0;
}

