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
	createBitMap(&allocator->bitmap, num_bits, buffer);
	for (int i = 0; i < num_bits; ++i) 
		setBit(&allocator->bitmap, i, 1);
	printf("Buddy Initializing\n");
	printf("\tlevels: %d", num_levels);
	printf("\tmin_bucket_size: %d bytes\n", min_bucket_size);
	printf("\tmanaged memory %d bytes\n", (1 << num_levels) * min_bucket_size);
}

int buddy_allocator_get_buddy(BuddyAllocator* allocator, int level) {
	int num_levels = allocator->num_levels;
	int min_bucket_size = allocator->min_bucket_size;
	// Verifica che il livello richiesto sia valido
	if (level < 0 || level >= num_levels) {
		printf("Livello non valido \n");
		return -1;
	}
	//Calcola l'indice del blocco richiesto nel livello specificato
	int idx = (1 << level) -1;
	int start = startIdx(idx);
	//Controlla se il blocco richiesto è disponibile
	if (testbit(&allocator->bitmap, idx) == 0) {
		printf("Blocco richiesto già assegnato \n");
		return -1;
	}
	//Trova l'indice del buddy richiesto
	int buddy_idx = buddyIdx(idx);
	//Verifica che il buddy sia disponibile
	if (testbit(&allocator->bitmap, buddy_idx) == 0) {
		printf("Il buddy del blocco richiesto non è disponibile\n");
		return -1;
	}
	//Marca i blocchi come assegnati
	setBit(&allocator->bitmap, idx, 0);
	setBit(&allocator->bitmap, buddy_idx, 0);
	//Calcola l'indirizzo di memoria del blocco richiesto
	char* memory = allocator->memory;
	int block_size = (1 << (num_levels - level)) * min_bucket_size;
	int offset = start * min_bucket_size;
	char* block_address = memory + offset;
	//Stampa per informazioni sul blocco assegnato
	printf("Blocco assegnato:\n");
	printf("\tLivello: %d\n", level);
	printf("\tDimensione: %d bytes\n", block_size);
	printf("\tIndirizzo: %p\n", block_address);
	return idx;
}

void BuddyAllocator_releaseBuddy(BuddyAllocator* alloc, int index) {
	int num_levels = alloc->num_levels;
	int min_bucket_size = alloc->min_bucket_size;
	//Verifica 	che l'indice sia valido
	if (index <  0 || index >= (1 << num_levels) -1) {
		printf("Indice non valido.\n");
		return;
	}
	//Controlla se il blocco è già stato rilasciato
	if (testbit(&alloc->bitmap, index) == 0) {
		printf("Il blocco è già stato rilasciato\n");
		return;
	}
	//Rilascia il blocco impostando il bit corrispondente a 1
	setBit(&alloc->bitmap, index, 1);
	//Verifica se il buddy può essere unito al blocco rilasciato
	while (1) {
		int buddy_idx = buddyIdx(index);
		//Verifica se il buddy è disponibile
		int buddy_available = ((buddy_idx < (1 << num_levels) -1) && (testbit(&alloc->bitmap, buddy_idx) == 1));
		if (!buddy_available) break;
		//Rilascia il blocco buddy impostando il bit corrispondente a 1
		setBit(&alloc->bitmap, buddy_idx, 1);
		//Calcola l'indice del padre comune dei due blocchi
		index = parentIdx(index);
	}
	//Stampa informazioni sul blocco rilasciato
	char* memory = alloc->memory;
	int block_size = (1 << (num_levels - levelIdx(index))) * min_bucket_size;
	int offset = startIdx(index) * min_bucket_size;
	char* block_address = memory + offset;
	printf("Blocco rilasciato:\n");
	printf("\tIndice: %d\n", index);
	printf("\tLivello: %d\n", levelIdx(index));
	printf("\tDimensione: %d bytes\n", block_size);
	printf("\tIndirizzo: %p\n", block_address);
}	

void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {
	int num_levels = alloc->num_levels;
	int min_bucket_size = alloc->min_bucket_size;
	//int block_size = 1 << (num_levels - 1);
	//Verifica che la dimensione richiesta sia valida
	if (size > (1 << num_levels) * min_bucket_size) {
		printf("La dimensione richiesta supera la dimensione massima gestibile");
		return NULL;
	}
	//Cerca il livello appropriato che può contenere la dimensione richiesta
	int level = 0;
	while ((level < num_levels) && (((1 << level) * min_bucket_size) < size)) {
		level++;
	}
	//Cerca il primo blocco disponibile a livello corrente
	int index = (1 << level) - 1;
	while (index < (1 << num_levels) - 1) {
		if (testbit(&alloc->bitmap, index) == 1) {
			//Imposta il blocco come assegnato
			setBit(&alloc->bitmap, index, 0);
			printf("Blocco allocato:\n");
			printf("\tIndice: %d\n", index);
			printf("\tLivello: %d\n", level);
			printf("\tDimensione: %d bytes\n", (1 << level) * (min_bucket_size));
			printf("\tIndirizzo: %p\n", alloc->memory + (index * min_bucket_size));
			return alloc->memory + (index * min_bucket_size);
		}
		index++;
	}
	printf("Nessun blocco disponibile di dimensione sufficiente\n");
	return NULL;
}

void BuddyAllocator_free(BuddyAllocator* alloc, void* mem) {
	if (mem == NULL) {
		printf("Memoria nulla. Impossibile deallocare\n");
		return;
	}
	int min_bucket_size = alloc->min_bucket_size;
	int index = ((char*)mem - alloc->memory) / min_bucket_size;
	//Verifica che l'indice sia valido
	if (index < 0 || index >= (1 << alloc->num_levels)) {
		printf("Indice non valido. Impossible deallocare\n");
		return;
	}
	//Verifica che il blocco sia stato precedemente allocato
	if (testbit(&alloc->bitmap, index) == 1) {
		printf("Il blocco non è stato allocato. Impossibile deallocare\n");
		return;
	}
	while (1) {
		int buddyIndex = -1;
		int level = levelIdx(index);
		//Verifica se il blocco ha un buddy libero
		if (level < alloc->num_levels - 1) {
			int blockSize = 1 << level;
			int buddySize = 1 << (level + 1);
			if (index % buddySize == 0) {
				buddyIndex = index + blockSize;
			} else {
				buddyIndex = index - blockSize;
			}
			if (testbit(&alloc->bitmap, buddyIndex) == 1) {
				//Unisci il blocco con il buddy libero
				if (buddyIndex < index) {
					int temp = index;
					index = buddyIndex;
					buddyIndex = temp;
				}
				//Imposta il buddy libero
				setBit(&alloc->bitmap, buddyIndex, 1);
			} else {
				//Buddy non libero interrompi il ciclo
				break;
			}
		} else {
			//Siamo al livello più basso interrompi il ciclo
		}
	}
	//Imposta il blocco come libero
	setBit(&alloc->bitmap, index, 1);
	printf("Blocco deallocato:\n");
	printf("\tIndice: %d\n", index);
	printf("\tIndirizzo: %p\n", mem);
}
	
	
	
	
	
