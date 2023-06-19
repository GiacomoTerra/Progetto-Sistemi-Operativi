#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include "buddy_allocator.h"

int levelIdx(size_t idx) { return (int)floor(log2(idx)); }

int startIdx(int idx) { return (idx - (1 << levelIdx(idx))); }

int parentIdx(int idx) { return idx / 2; }

int buddyIdx(int idx) {
	if (idx & 0x1) 
		return idx -1;
	return idx + 1;
}

//initializza il buddy allocator
void init_buddy(BuddyAllocator *allocator, uint8_t* buffer, int num_levels, char* memory, int min_bucket_size)  {
	assert(num_levels < MAX_LEVELS);
	allocator->num_levels = num_levels;
	allocator->memory = memory;
	allocator->min_bucket_size = min_bucket_size;
	int num_bits = (1 << num_levels)-1;
	createBitMap(&allocator->bitmap, num_bits, uint8_t* buffer);
	for (int i = 0; i < num_bits; ++i) 
		setbit(&allocator->bitmap, i, 1);
	printf("Buddy Initializing\n");
	printf("\tlevels: %d", num_levels);
	printf("\tmin_bucket_size: %d bytes\n", min_bucket_size);
	printf("\tmanaged memory %d bytes\n", (1 << num_levels) * min_bucket_size);
}

void* allocate(BuddyAllocator* allocator, int size) {	
	int start_page = findBestFit(allocator, size);
	if (start_page == -1) return NULL;
	int num_pages_needed = (size + PAGE_SIZE -1) / PAGE_SIZE;
	int start_address = start_page * PAGE_SIZE;
	markPagesAllocated(allocator, start_page, num_pages_needed);
	return allocator->memory + start_address;
}

void deallocate(BuddyAllocator* allocator, void* ptr, int size) {
	if (ptr == NULL) return;
	int start_address = (uint8_t*)ptr - allocator->memory;
	int start_page = start_address / PAGE_SIZE;
	int num_pages_needed = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	markPagesFree(allocator, start_page, num_pages_needed);
}
	
	
	
	
	
