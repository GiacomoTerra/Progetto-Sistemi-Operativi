#include "bitmap.h"
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

BuddyAllocator* createBuddyAllocator(int memory_size) ;
void destroyBuddyAllocator(BuddyAllocator* allocator);
int findBestFit(BuddyAllocator* allocator, int size);
void markPagesAllocated(BuddyAllocator* allocator, int start_page, int num_pages);
void markPagesFree(BuddyAllocator* allocator, int start_page, int num_pages);
void* allocate(BuddyAllocator* allocator, int size);
void deallocate(BuddyAllocator* allocator, void* ptr, int size);
