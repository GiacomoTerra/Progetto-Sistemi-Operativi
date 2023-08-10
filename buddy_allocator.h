#include <stddef.h>
#include "bitmap.h"
#pragma once

typedef struct {
	Bitmap bitmap;	
	char* memory;
	int num_levels;
	int min_bucket_size;
} BuddyAllocator;

int levelIdx(size_t idx);
int startIdx(int idx);
int parentIdx(int idx);
int buddyIdx(int idx);
int find_first_free_block(Bitmap* bitmap, int level);
void init_buddy(BuddyAllocator *allocator, uint8_t* buffer, int num_levels, char* memory, int min_bucket_size);
int buddy_allocator_get_buddy(BuddyAllocator* allocator, int level);
void BuddyAllocator_releaseBuddy(BuddyAllocator* allocator, int idx);
void* BuddyAllocator_malloc(BuddyAllocator* allocator, int size);
void BuddyAllocator_free(BuddyAllocator* allocator, void* mem);
