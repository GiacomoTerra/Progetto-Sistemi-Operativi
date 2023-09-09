#include "pseudo_malloc.h"
#include <assert.h>
#include <stdio.h>
#define NUM_BLOCKS 1000

int main()
{

	pseudo_init();
	printf("Allocatore inizzializzato correttamente\n");	
	
	// Allocare una piccola quantità di memoria
	void* small_mem = pseudo_malloc(5);
	assert(small_mem != NULL);
	pseudo_free(small_mem, 5);
	printf("\n\n");
	// Allocare una grande quantità di memoria (utilizza mmap)
	void* large_mem = pseudo_malloc(PAGE_SIZE / 2);
	assert(large_mem != NULL);
	pseudo_free(large_mem, PAGE_SIZE / 2);
	printf("\n\n");
	// Allocare e liberare in ordine casuale
	void* mem1 = pseudo_malloc(20);
	printf("\n\n");
	pseudo_free(mem1, 20);
	void* mem2 = pseudo_malloc(42);
	pseudo_free(mem2, 42);
	printf("\n\n");
	void* mem3 = pseudo_malloc(64);
	pseudo_free(mem3, 64);
	printf("\n\n");
	// Libera la memoria di medie dimensioni
	printf("\n\n");
	void* mem4 = pseudo_malloc(32);
	printf("\n\n");
	pseudo_free(mem4, 32);
	
	/*void* ptr1 = pseudo_malloc(20);
	void* ptr2 = pseudo_malloc(30);
	void* ptr3 = pseudo_malloc(40);
	void* ptr4 = pseudo_malloc(50);
	void* ptr5 = pseudo_malloc(60);
	void* ptr6 = pseudo_malloc(60);
	void* ptr7 = pseudo_malloc(8);
	void* ptr8 = pseudo_malloc(9);
	void* ptr9 = pseudo_malloc(4);
	pseudo_free(ptr1, 20);
	pseudo_free(ptr2, 30);
	pseudo_free(ptr3, 40);
	pseudo_free(ptr4, 50);
	pseudo_free(ptr5, 60);
	pseudo_free(ptr6, 60);
	pseudo_free(ptr7, 8);
	pseudo_free(ptr8, 9);
	pseudo_free(ptr9, 4);*/
	
}
