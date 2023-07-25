#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include "buddy_allocator.h"

int levelIdx(size_t idx) { return (int)(floor(log2(idx))); }

int startIdx(int idx) { return (idx - (1 << levelIdx(idx))); }

int parentIdx(int idx) { return idx / 2; }

int buddyIdx(int idx)
{
	if (idx & 0x1)
		return idx - 1;
	return idx + 1;
}
// Funzione che imposta come assegnati i figli di un nodo
void set_childrens_assigned(Bitmap* bitmap, int idx, int level) {
	// Caso base siamo all'ultimo livello
	if (level <= 0) {
		return;
	}
	int left_child = idx * 2;
	int right_child = (idx*2) + 1;
	setBit(bitmap, left_child, 0);
	setBit(bitmap, right_child, 0);
	set_childrens_assigned(bitmap, left_child, level - 1);
	set_childrens_assigned(bitmap, right_child, level -1);
}

// Funzione che imposta come assegnati i padri di un nodo
void set_parents_assigned(Bitmap* bitmap, int idx) {
	int parent = parentIdx(idx);
	while (parent >= 1) {
		setBit(bitmap, parent, 0);
		parent = parentIdx(parent);
	}
}

// Funzione che imposta come liberi i figli di un blocco rilasciato
void set_childrens_free(Bitmap* bitmap, int idx, int level) {
	// Caso base siamo all'ultimo livello
	if (level <= 0) {
		return;
	}
	// Calcolo indici nodi figli
	int left_child = idx * 2;
	int right_child = (idx*2) + 1;
	// Imposto i blocchi figli come disponibili 
	setBit(bitmap, left_child, 1);
	setBit(bitmap, right_child, 1);
	set_childrens_assigned(bitmap, left_child, level - 1);
	set_childrens_assigned(bitmap, right_child, level -1);
}
// initializza il buddy allocator
void init_buddy(BuddyAllocator *allocator, uint8_t *buffer, int num_levels, char *memory, int min_bucket_size)
{
	assert(num_levels < MAX_LEVELS);
	allocator->num_levels = num_levels;
	allocator->memory = memory;
	allocator->min_bucket_size = min_bucket_size;
	int num_bits = (1 << num_levels) - 1;
	createBitMap(&allocator->bitmap, num_bits, buffer);
	for (int i = 0; i < num_bits; ++i)
		setBit(&allocator->bitmap, i, 1);
	printf("Buddy Initializing\n");
	printf("\tlevels: %d", num_levels);
	printf("\tmin_bucket_size: %d bytes\n", min_bucket_size);
	printf("\tmanaged memory %d bytes\n", (1 << num_levels) * min_bucket_size);
}

// Funzione che ritorna l'indice del buddy richiesto
int buddy_allocator_get_buddy(BuddyAllocator *allocator, int level)
{
	int num_levels = allocator->num_levels;
	int min_bucket_size = allocator->min_bucket_size;
	// Verifica che il livello richiesto sia valido
	if (level < 0 || level >= num_levels)
	{
		printf("Livello non valido \n");
		return -1;
	}
	// Calcola l'indice del blocco richiesto nel livello specificato
	int idx = (1 << level) - 1;
	int start = startIdx(idx);
	// Controlla se il blocco richiesto è disponibile
	if (testbit(&allocator->bitmap, idx) == 0)
	{
		// Blocco assegnato Cerco se il buddy è libero
		printf("Blocco richiesto già assegnato cerco se il buddy è libero\n");
		// Trova l'indice del buddy richiesto
		int buddy_idx = buddyIdx(idx);
		// Verifica che il buddy sia disponibile
		if (testbit(&allocator->bitmap, buddy_idx) == 0) {
			printf("Il buddy del blocco richiesto non è disponibile\n");
			return -1;
		}
		else {			
			// Imposto come assegnato il buddy
			setBit(&allocator->bitmap, buddy_idx, 0);
			// Imposto come assegnati tutti i suoi eventuali figli
			set_childrens_assigned(&allocator->bitmap, buddy_idx, level);
			// Imposto come assegnati tutti i suoi eventuali padri
			set_parents_assigned(&allocator->bitmap, buddy_idx);
			printf("Il blocco richiesto è già assegnato. Ritorna il buddy\n");
			return buddy_idx;
		}
	}
	// 	Imposto come assegnato il blocco all'indice richiesto
	setBit(&allocator->bitmap, idx, 0);
	// Imposto come assegnati tutti i suoi eventuali figli
	set_childrens_assigned(&allocator->bitmap, idx, level);
	// Imposto come assegnati tutti i suoi eventuali padri
	set_parents_assigned(&allocator->bitmap, idx);
	// Calcola l'indirizzo di memoria del blocco richiesto
	char *memory = allocator->memory;
	int block_size = (1 << (num_levels - level)) * min_bucket_size;
	int offset = start * min_bucket_size;
	char *block_address = memory + offset;
	// Stampa per informazioni sul blocco assegnato
	printf("Blocco assegnato:\n");
	printf("\tLivello: %d\n", level);
	printf("\tDimensione: %d bytes\n", block_size);
	printf("\tIndirizzo: %p\n", block_address);
	return idx;
}

void BuddyAllocator_releaseBuddy(BuddyAllocator *alloc, int index)
{
	int num_levels = alloc->num_levels;
	int min_bucket_size = alloc->min_bucket_size;
	// Verifica 	che l'indice sia valido
	if (index < 0 || index >= (1 << num_levels) - 1)
	{
		printf("Indice non valido.\n");
		return;
	}
	// Controlla se il blocco è già stato rilasciato
	if (testbit(&alloc->bitmap, index) == 1)
	{
		printf("Il blocco è già stato rilasciato\n");
		return;
	}
	// Rilascia il blocco impostando il bit corrispondente a 1
	setBit(&alloc->bitmap, index, 1);
	// Calcolo il livello dell'indice richiesto
	int level = levelIdx(index);
	// Rilascio tutti i blocchi figli che stanno sotto
	set_childrens_free(&alloc->bitmap, index, level);
	// Verifica se il buddy può essere unito al blocco rilasciato
	while (1)
	{
		// Calcola l'indice del blocco buddy
		int buddy_idx = buddyIdx(index);
		// Calcola l'indice del blocco parent
		int parent_idx = parentIdx(index);
		// Verifica se il buddy è disponibile
		int buddy_available = ((buddy_idx < (1 << num_levels) - 1) && (testbit(&alloc->bitmap, buddy_idx) == 1));
		if (!buddy_available)
			break;
		// Rilascia il blocco padre impostando il bit corrispondente a 1
		setBit(&alloc->bitmap, parent_idx, 1);
		// Imposta l'indice come quello del parent per salire di un livello		
		index = parent_idx;
	}
	// Stampa informazioni sul blocco rilasciato
	char *memory = alloc->memory;
	int block_size = (1 << (num_levels - levelIdx(index))) * min_bucket_size;
	int offset = startIdx(index) * min_bucket_size;
	char *block_address = memory + offset;
	printf("Blocco rilasciato:\n");
	printf("\tIndice: %d\n", index);
	printf("\tLivello: %d\n", levelIdx(index));
	printf("\tDimensione: %d bytes\n", block_size);
	printf("\tIndirizzo: %p\n", block_address);
}

void *BuddyAllocator_malloc(BuddyAllocator *alloc, int size)
{
	int num_levels = alloc->num_levels;
	int min_bucket_size = alloc->min_bucket_size;
	int max_block_size = (1 << num_levels) * min_bucket_size;
	// int block_size = 1 << (num_levels - 1);
	// Verifica che la dimensione richiesta sia valida
	if (size > max_block_size)
	{
		// Allocazione Fallita
		printf("La dimensione richiesta supera la dimensione massima gestibile\n");
		return NULL;
	}
	// Cerca il livello appropriato che può contenere la dimensione richiesta
	int level = 0;
	while ((level < num_levels) && (((1 << level) * min_bucket_size) < size))
	{
		level++;
	}
	// Ottieni il blocco dal buddy allocator
	int idx = buddy_allocator_get_buddy(alloc, level);
	if (idx == -1) {
		// Allocazione Fallita
		printf("Impossibile trovare spazio necessario\n");
		return NULL;
	}
	// Calcola l'indirizzo di memoria del blocco assegnato
	int start = startIdx(idx);
	char* mem = alloc->memory + start * min_bucket_size;
	int block_size = (1 << level) * (min_bucket_size);
	// Restituisce il puntatore al blocco di memoria ottenuto dal buddy allocator
	printf("Blocco allocato dal buddy allocator\n");
	printf("\tLivello: %d\n", level);
	printf("\tDimensione: %d bytes\n", block_size);
	printf("\tIndirizzo: %p\n", mem);
	return mem;
}

void BuddyAllocator_free(BuddyAllocator *alloc, void *mem)
{
	if (mem == NULL)
	{
		printf("Memoria nulla. Impossibile deallocare\n");
		return;
	}
	char* memory = (char*)mem;
	int min_bucket_size = alloc->min_bucket_size;
	int idx = (memory - alloc->memory) / min_bucket_size;
	int num_levels = alloc->num_levels;
	// Verifica che l'indice sia valido
	if (idx < 0 || idx >= (1 << num_levels))
	{
		printf("Indice non valido. Impossible deallocare\n");
		return;
	}
	// Rilascia il blocco nel buddy allocator
	BuddyAllocator_releaseBuddy(alloc, idx);
	printf("Blocco deallocato:\n");
	printf("\tIndice: %d\n", idx);
	printf("\tIndirizzo: %p\n", mem);
}
