#include "pseudo_malloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

char memory[MEMORY_SIZE];
uint8_t bitmap_buffer[(1 << (BUDDY_LEVELS + 1))];
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

