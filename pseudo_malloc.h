#pragma once
#include "buddy_allocator.h"

#define PAGE_SIZE 4096
#define BUDDY_LEVELS 7
#define MEMORY_SIZE 1024 * 1024
#define MIN_BUCKET_SIZE (MEMORY_SIZE >> (BUDDY_LEVELS - 1))
#define BITMAP_BUFFER_SIZE (1 << (BUDDY_LEVELS + 1))

void pseudo_init();
void* pseudo_malloc(int size);
void pseudo_free(void* mem, int size);
void* pseudo_realloc(void* mem, int old_size, int new_size);
