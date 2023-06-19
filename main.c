#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "buddy_allocator.c"

#define PAGE_SIZE 4096
#define BUDDY_THRESHOLD (PAGE_SIZE/4)
#define BITMAP_SIZE_IN_BYTES 1024 * 1024
uint8_t* buffer[BITMAP_SIZE_IN_BYTES];


void BitMap_prova() {
	int num_bits = BITMAP_SIZE_IN_BYTES * 8;
	BitMap bitmap
	createBitMap(&bitmap, num_bits, buffer);
	setBit(BitMap, 500, 1);
	int bitValue = getBit(BitMap, 500);
	printf("Bit value at index 500: %d\n", bitValue);
	BitMap_print(BitMap);
	destroyBitMap(BitMap);
	return;
}

void BuddyAllocator_prova() {
	int memory_size = 1 * 1024 * 1024;
	BuddyAllocator* allocator = createBuddyAllocator(memory_size);
	int* ptr = (int*)allocate(allocator, sizeof(int));
	if (ptr == NULL) printf("Failed to allocate memory for an integer\n");
	else {
		*ptr = 42;
		printf("Allocated integer: %d\n", *ptr);
	}
	deallocate(allocator, ptr, sizeof(int));
	destroyBuddyAllocator(allocator);
	return;
}

void* find_fit(size_t size) {
	size_t* header = heap_start();
	while (header < heap_
