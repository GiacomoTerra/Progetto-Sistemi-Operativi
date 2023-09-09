#include <stddef.h>
#include "bitmap.h"
#pragma once

#define LEVELS 7
#define BUDDY_SIZE 1024
#define MIN_SIZE (BUDDY_SIZE >> (LEVELS - 1))

typedef struct {
	Bitmap bitmap;	
	char* memory;
	int num_levels;
	int min_bucket_size;
} BuddyAllocator;

typedef struct {
	int buddy_index;
} MemoryBlockMetadata;

int levelIdx(size_t idx);
int startIdx(int idx);
int parentIdx(int idx);
int is_valid_index(int i, int max);
void clearAllBits(Bitmap* bitmap);
int get_buddy_index(int idx);
int find_first_free_block(BuddyAllocator *allocator, int level);
int get_level(BuddyAllocator *allocator, int size);
void *get_memory(BuddyAllocator *allocator, int level, int idx);
void init_buddy(BuddyAllocator *allocator, uint8_t* buffer, int num_levels, char* memory, int min_bucket_size);
int buddy_allocator_get_buddy(BuddyAllocator* allocator, int level);
void BuddyAllocator_releaseBuddy(BuddyAllocator* allocator, int idx);
void* BuddyAllocator_malloc(BuddyAllocator* allocator, int size);
void BuddyAllocator_free(BuddyAllocator* allocator, void* mem);
void print_metadata(void* mem);
int get_level_from_index(int idx);
