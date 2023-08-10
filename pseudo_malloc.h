#pragma once
#include "buddy_allocator.h"

#define PAGE_SIZE 4096
#define BUDDY_LEVELS 9
#define MEMORY_SIZE 1024*1024
#define MIN_BUCKET_SIZE (MEMORY_SIZE >> BUDDY_LEVELS)

void pseudo_init();
void* pseudo_malloc(int size);
void pseudo_free(void* mem, int size);
