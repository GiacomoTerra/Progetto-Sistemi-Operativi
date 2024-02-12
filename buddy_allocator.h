#include <stddef.h>
#include "bitmap.h"
#pragma once


typedef struct {
	Bitmap bitmap;	// BitMap per gestire i blocchi in memoria
	char* memory;	// puntatore alla memoria gestita dall'allocatore
	int num_levels; // numero di livelli nell'albero del buddy allocator
	int min_bucket_size; // la dimensione minima del blocco di memoria gestibile
} BuddyAllocator;

typedef struct {
	int buddy_index; // index del buddy
} BuddyBlockHeader;

int levelIdx(size_t idx);
int startIdx(int idx);
int parentIdx(int idx);
int is_valid_index(int i, int max);
void clearAllBits(Bitmap* bitmap);
int get_buddy_index(int idx);
int find_first_free_block(const Bitmap *bitmap, int level);
int get_level(BuddyAllocator *allocator, int size);
int find_and_divide_iterative(BuddyAllocator* allocator, int level);
BuddyBlockHeader* get_memory(BuddyAllocator *allocator, int level, int idx);
void init_buddy(BuddyAllocator *allocator, uint8_t* buffer, int num_levels, char* memory, int min_bucket_size);
int buddy_allocator_get_buddy(BuddyAllocator* allocator, int level);
void BuddyAllocator_releaseBuddy(BuddyAllocator* allocator, int idx);
void* BuddyAllocator_malloc(BuddyAllocator* allocator, int size);
void BuddyAllocator_free(BuddyAllocator* allocator, void* mem);
void print_metadata(void* mem);
int get_level_from_index(int idx);
