#include "pseudo_malloc.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

int main()
{

	pseudo_init();
	printf("Allocatore inizzializzato correttamente\n");	
	
	
	// Allocare una piccola quantità di memoria
	/*void* small_mem = pseudo_malloc(5);
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
	pseudo_free(ptr7, 175);*/
	
	printf("Test 1: Allocazione e Deallocazione con pseudo_malloc e pseudo_free\n");
	
	int* arr = (int*) pseudo_malloc(10 * sizeof(int));
	for (int i = 0; i < 10; i++) {
		arr[i] = i;
	}
	for (int i = 0; i < 10; i++) {
		printf("%d ", arr[i]);
	}
	printf("\n");
	pseudo_free(arr, 10 * sizeof(int));
	
	printf("Test 2: Realloc con riduzione della dimensione\n");
	
	char* string = (char*) pseudo_malloc(20);
	strcpy(string, "Hello World!");
	printf("Prima: %s\n", string);
	string = (char*) pseudo_realloc(string, 20, 10);
	printf("Dopo: %s\n", string);
	pseudo_free(string, 10);
	
	printf("Test 3: Realloc con aumento della dimensione\n");
	
	char* string2 = (char*) pseudo_malloc(10);
	strcpy(string2, "Hello");
	printf("Prima; %s\n", string2);
	string2 = (char*) pseudo_realloc(string2, 10, 20);
	strcat(string2, ", World!");
	printf("Dopo: %s\n", string2);
	pseudo_free(string2, 20);
	
}
