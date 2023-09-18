#include "pseudo_malloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

char memory[MEMORY_SIZE];
uint8_t bitmap_buffer[BITMAP_BUFFER_SIZE];
BuddyAllocator allocator;

void pseudo_init() {
	init_buddy(&allocator, bitmap_buffer, BUDDY_LEVELS, memory, MIN_BUCKET_SIZE);
}

void* pseudo_malloc(int size) {
	if (size <= 0) {
		printf("Grandezza nulla, inserisci una grandezza valida\n");
		return NULL;
	}
	if (size >= PAGE_SIZE / 4) {
		//Richiesta superiore a 1/4 della page_size, utilizza mmap
		void* mem = mmap(0, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (mem == MAP_FAILED) {
			printf("Errore durante l'allocazione della memoria con mmap\n");
			return NULL;
		}
		printf("Memoria allocata con mmap:\n");
		printf("\tDimensione: %d bytes\n", size);
		printf("\tIndirizzo: %p\n", mem);
		return mem;
	} else {
		//Richiesta inferiore a 1/4 della page size
		return BuddyAllocator_malloc(&allocator, size);
	}
}

void pseudo_free(void* mem, int size) {
	if (mem == NULL || size <= 0) {
		printf("Memoria nulla, impossibile deallocare\n");
		return;
	}
	if (size >= PAGE_SIZE / 4) {
		//Dimensione superiore a 1/4 della page size, utilizza munmap
		int res = munmap(mem, size);
		if (res == -1) {
			printf("Errore durante la deallocazione della memoria con munmap\n");
			return;
		}
		printf("Memoria deallocata con munmap:\n");
		printf("\tDimensione: %d bytes\n", size);
		printf("\tIndirizzo: %p\n", mem);
	} else {
		//Dimensione inferiore a 1/4 della page size
		BuddyAllocator_free(&allocator, mem);
	}
}

void* pseudo_realloc(void* mem, int old_size, int new_size) {
	// Se la new size è 0 liberiamo la memoria e restituiamo NULL
	if (new_size == 0) {
		pseudo_free(mem, old_size);
		return NULL;
	} else if (!mem) {
		// Se mem è NULL allochiamo un nuovo blocco di memoria
		return pseudo_malloc(new_size);
	} else if (new_size <= old_size) {
		// Se la nuova dimensione è minore o uguale alla vecchia dimensione 
		// semplicemente restituisci il puntatore senza fare nulla
		return mem;
	} else {
		void* new_mem = pseudo_malloc(new_size);
		if (new_mem) {
			// Copia i dati del vecchio blocco di memoria al nuovo
			memcpy(new_mem, mem, old_size);
			pseudo_free(mem, old_size);
		}
		return new_mem;
	}
}
		
