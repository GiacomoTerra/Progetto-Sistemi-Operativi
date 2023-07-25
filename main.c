#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "buddy_allocator.h"


#define BUDDY_LEVELS 10
#define MEMORY_SIZE (1 << 20) 
#define MIN_BUCKET_SIZE (MEMORY_SIZE>>(BUDDY_LEVELS-1))
#define PAGE_SIZE 4096
#define BITMAP_BUFFER_SIZE (1 << (BUDDY_LEVELS+1))
#define BUDDY_THRESHOLD (PAGE_SIZE / 4)
#define BITMAP_SIZE_IN_BYTES 256

char memory[MEMORY_SIZE];
uint8_t bitmap_buffer[BITMAP_BUFFER_SIZE];

/*void BitMap_prova()
{
	int num_bits = BITMAP_SIZE_IN_BYTES * 8;
	Bitmap *bitmap = malloc(sizeof(Bitmap));
	createBitMap(bitmap, num_bits, buffer);
	setBit(bitmap, 0, 1);
	setBit(bitmap, 8, 1);
	setBit(bitmap, 16, 1);
	int bitValue1 = getBit(bitmap, 0);
	printf("Bit value at index 0: %d\n", bitValue1);
	int bitValue2 = getBit(bitmap, 8);
	printf("Bit value at index 8: %d\n", bitValue2);
	int bitValue3 = getBit(bitmap, 16);
	printf("Bit value at index 16: %d\n", bitValue3);
	BitMap_print(bitmap);
	destroyBitMap(bitmap);
	return;
}*/

void BuddyAllocator_prova()
{
	// int memory_size = 1 * 8 * 8;
	BuddyAllocator *allocator = malloc(sizeof(BuddyAllocator));
	//char *mem = malloc(sizeof(char) * BITMAP_SIZE_IN_BYTES);	
	init_buddy(allocator, bitmap_buffer, BUDDY_LEVELS, memory, MIN_BUCKET_SIZE);
	int *ptr = (int *)BuddyAllocator_malloc(allocator, sizeof(int));
	if (ptr == NULL)
		printf("Failed to allocate memory for an integer\n");
	else
	{
		*ptr = 123456798;
		printf("Allocated integer: %d\n", *ptr);
	}

	printf("\n\n");
	BitMap_print(&allocator->bitmap);
	printf("\n\n");

	int *ptr2 = (int *)BuddyAllocator_malloc(allocator, sizeof(int));
	if (ptr2 == NULL)
		printf("Failed to allocate memory for an integer\n");
	else
	{
		*ptr2 = 123456798;
		printf("Allocated integer: %d\n", *ptr2);
	}

	printf("\n\n");
	int x = *ptr + *ptr2;
	printf("%d somma", x);

	printf("\n\n");
	BitMap_print(&allocator->bitmap);
	printf("\n\n");

	int *ptr3 = (int *)BuddyAllocator_malloc(allocator, sizeof(int));
	if (ptr3 == NULL)
		printf("Failed to allocate memory for an integer\n");
	else
	{
		*ptr3 = 123456798;
		printf("Allocated integer: %d\n", *ptr3);
	}

	printf("\n\n");
	x = x + *ptr3;
	printf("%d somma", x);

	printf("\n\n");
	BitMap_print(&allocator->bitmap);
	printf("\n\n");

	printf("\n\n");
	printf("%d\t", *ptr);
	printf("\n\n");
	printf("%d\t", *ptr2);
	printf("\n\n");
	printf("%d\t", *ptr3);
	printf("\n\n");

	int bitValue1 = getBit(&allocator->bitmap, 0);
	printf("Bit value at index 0: %d\n", bitValue1);
	int bitValue2 = getBit(&allocator->bitmap, 9);
	printf("Bit value at index 9: %d\n", bitValue2);
	int bitValue3 = getBit(&allocator->bitmap, 18);
	printf("Bit value at index 18: %d\n", bitValue3);

	BuddyAllocator_free(allocator, ptr);
	BuddyAllocator_free(allocator, ptr2);
	BuddyAllocator_free(allocator, ptr3);
	free(allocator);
	return;
}

int main()
{
	printf("ciao\n");
	// BitMap_prova();
	BuddyAllocator_prova();
	return 0;
}
