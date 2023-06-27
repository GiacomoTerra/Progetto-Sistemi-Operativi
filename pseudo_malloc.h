#pragma once
#include "buddy_allocator.c"

#define PAGE_SIZE 4096

void* pseudo_malloc(BuddyAllocator* alloc, int size);
void pseudo_free(BuddyAllocator* alloc, void* mem);
