#include "bitmap.c"
#pragma once

// Dimensione della pagina in byte
#define PAGE_SIZE 4096
#define MAX_LEVELS 18

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
void init_buddy(BuddyAllocator *allocator, uint8_t* buffer, int num_levels, char* memory, int min_bucket_size);
int buddy_allocator_get_buddy(BuddyAllocator* allocator, int level);
void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, int index);
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size);
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);
