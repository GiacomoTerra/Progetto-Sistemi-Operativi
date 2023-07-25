#pragma once
#include "buddy_allocator.h"

#define PAGE_SIZE 4096

void* pseudo_malloc(BuddyAllocator* alloc, int size);
void pseudo_free(BuddyAllocator* alloc, void* mem);
