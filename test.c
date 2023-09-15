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
	void* large_mem_x2 = pseudo_malloc(PAGE_SIZE);
	assert(large_mem_x2 != NULL);
	pseudo_free(large_mem_x2, PAGE_SIZE);
	printf("\n\n");
	// Allocare e liberare in ordine casuale
	void* mem1 = pseudo_malloc(20);
	printf("\n\n");
	void* mem2 = pseudo_malloc(40);
	printf("\n\n");
	void* mem3 = pseudo_malloc(80);
	printf("\n\n");
	pseudo_free(mem2, 40);
	printf("\n\n");
	void* mem4 = pseudo_malloc(100);
	printf("\n\n");
	pseudo_free(mem1, 20);	
	printf("\n\n");
	pseudo_free(mem3, 80);
	printf("\n\n");
	pseudo_free(mem4, 100);
	// Tentativo di allocare 0 byte
	void* mem = pseudo_malloc(0);
	assert(mem == NULL);
	// Tentativo di liberare un puntatore a NULL
	pseudo_free(NULL, 100);
	// Test di allocazione di base
	void* ptr1 = pseudo_malloc(100);
	assert(ptr1 != NULL);
	// Test di allocazione consecutiva
	void* ptr2 = pseudo_malloc(50);
	assert(ptr2 != NULL);
	void* ptr3 = pseudo_malloc(25);
	// Test di liberazione della memoria
	pseudo_free(ptr2, 50);
	void* ptr4 = pseudo_malloc(50);
	assert(ptr4 != NULL);
	// Test di liberazione e riallocazione
	pseudo_free(ptr3, 25);
	void* ptr5 = pseudo_malloc(25);
	assert(ptr5 == ptr3);  // Dovrebbe allocare lo stesso blocco
	// Test di allocazione dopo la liberazione
	pseudo_free(ptr1, 100);
	pseudo_free(ptr4, 50);
	pseudo_free(ptr5, 25);
	// Tentativo di allocare la somma totale 
	void* ptr7 = pseudo_malloc(175);
	assert(ptr7 != NULL);
	pseudo_free(ptr7, 175);
	
	/*void* ptrs[20];
	for (int i = 0; i < 20; ++i) {
		ptrs[i] = (void*)pseudo_malloc(4);
	}
	for (int i = 0; i < 20; ++i) {
		pseudo_free(ptrs[i], 4);
	}*/
}
