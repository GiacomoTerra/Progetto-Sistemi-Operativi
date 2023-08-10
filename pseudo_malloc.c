#include "pseudo_malloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

void* pseudo_malloc(BuddyAllocator* alloc, int size) {
	if (size <= 0) {
		printf("Grandezza nulla, inserisci una grandezza valida\n");
		return NULL;
	}
	if (size > PAGE_SIZE / 4) {
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
		return BuddyAllocator_malloc(alloc, size);
	}
}

void pseudo_free(BuddyAllocator* alloc, void* mem) {
	if (mem == NULL) {
		printf("Memoria nulla, impossibile deallocare\n");
		return;
	}
	int min_bucket_size = alloc->min_bucket_size;
	int index = ((char*)mem - alloc->memory) / min_bucket_size;
	//Verifica che l'indice sia valido
	if (index < 0 || index >= (1 << alloc->num_levels)) {
		printf("Indice non valido, impossibile deallocare\n");
		return;
	}
	if (min_bucket_size > PAGE_SIZE / 4) {
		//Dimensione superiore a 1/4 della page size, utilizza munmap
		int result = munmap(mem, min_bucket_size);
		if (result == -1) {
			printf("Errore durante la deallocazione della memoria con munmap\n");
			return;
		}
		printf("Memoria deallocata con munmap:\n");
		printf("\tIndirizzo: %p\n", mem);
	} else {
		//Dimensione inferiore a 1/4 della page size
		BuddyAllocator_free(alloc, mem);
	}
}

