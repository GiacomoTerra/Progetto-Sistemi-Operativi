#include "pseudo_malloc.h"
#include <assert.h>
#include <stdio.h>

int main()
{
	// initialize the buddy allocator
	pseudo_init();
	printf("Allocatore inizzializzato correttamente\n");
	// --------------------------------------------------

	printf("TEST CASE 1: buddy_allocator_malloc and buddy_allocator_free\n");
	int *ptr1 = (int *)pseudo_malloc(sizeof(int));
	assert(ptr1 != NULL);
	printf("Allocated memory: %p\n\n", ptr1);

	int *ptr2 = (int *)pseudo_malloc(2 * sizeof(int));
	assert(ptr2 != NULL);
	printf("Allocated memory: %p\n\n", ptr2);

	int *ptr3 = (int *)pseudo_malloc((1 << 10) - 1);
	assert(ptr3 != NULL);
	printf("Allocated memory: %p\n\n", ptr3);

	pseudo_free(ptr1, sizeof(int));
	printf("Freed memory: %p\n\n", ptr1);

	pseudo_free(ptr2, 2 * sizeof(int));
	printf("Freed memory: %p\n\n", ptr2);

	pseudo_free(ptr3, (1 << 10) - 1);
	printf("Freed memory: %p\n\n", ptr3);

	// --------------------------------------------------

	printf("\n\nTEST CASE 2: mmap and munmap\n");
	ptr1 = (int *)pseudo_malloc(1 << 10);
	assert(ptr1 != NULL);
	printf("Allocated memory: %p\n", ptr1);

	ptr2 = (int *)pseudo_malloc(1 << 10);
	assert(ptr2 != NULL);
	printf("Allocated memory: %p\n", ptr2);

	pseudo_free(ptr1, 1 << 10);
	printf("Freed memory: %p\n", ptr1);

	pseudo_free(ptr2, 1 << 10);
	printf("Freed memory: %p\n", ptr2);

	// --------------------------------------------------

	printf("\n\nTEST CASE 3: allocate all the available memory of the buddy allocator and freeing invalid addresses\n");

	int *ptrs[20];
	for (int i = 0; i < 20; ++i)
	{
		ptrs[i] = (int *)pseudo_malloc(1);
	}

	for (int i = 0; i < 20; ++i)
	{
		pseudo_free(ptrs[i], 1);
	}

	// --------------------------------------------------

	printf("\n\nTEST CASE 4: trying to free an invalid address with the munmap\n");

	ptr1 = (int *)pseudo_malloc(1 << 10);
	printf("Allocated memory: %p\n", ptr1);

	pseudo_free(ptr1 + 1, 1 << 10);
}
