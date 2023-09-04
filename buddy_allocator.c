#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include "buddy_allocator.h"

int levelIdx(size_t idx) { return (int)(floor(log2(idx))); }

int startIdx(int idx) { return (idx - (1 << levelIdx(idx))); }

int parentIdx(int idx) { 
	return (idx - 1) / 2; 
}

// Funzione ausiliaria per controllare se un indice è valido
int is_valid_index(int i, int max) {
	return i >= 0 && i < max;
}

int get_buddy_index(int idx) {
	if (idx % 2 == 0)
		return idx - 1;
	else 
		return idx + 1;
}

void clearAllBits(Bitmap* bitmap) {
	for (int i = 0; i < bitmap->buffer_size; i++) {
		bitmap->buffer[i] = 0;
	}
}

//  Funzione che dato il livello cerca il primo blocco libero
int find_first_free_block(BuddyAllocator *allocator, int level) {
	assert(level < allocator->num_levels);
	int start_idx = (1 << level) - 1;
	int end_idx = (1 << (level + 1)) -1;
	for (int idx = start_idx; idx < end_idx; idx++) {
		if (getBit(&allocator->bitmap, idx) == 1)
			return idx;
	}
	// Nessun blocco libero
	return -1;
}

// Funzione che ritorna il livello del più piccolo blocco che può contenere i bytes
int get_level(BuddyAllocator *allocator, int size) {
	int level = 0;
	int block_size = allocator->min_bucket_size;
	while (block_size < size) {
		block_size *= 2;
		level++;
	}
	return allocator->num_levels - level - 1;
}

// Funzione che ritorna l'indirizzo di memoria del buddy
void *get_memory(BuddyAllocator *allocator, int level, int idx) {
	// Verifica che il livello e l'indice siano validi
	if (level < 0 || level >= allocator->num_levels || idx < 0 || idx >= (1 << level)) {
		printf("Errore: livello o indice non validi\n");
		return NULL;
	}
	int block_size = allocator->min_bucket_size * (1 << (allocator->num_levels - 1 - level));
	// Calcola l'offset in memoria basato sull'indice del blocco
	int start = (1 << level) -1;
	int offset = (idx - start) * block_size;
	// Ritorna l'indirizzo di memoria corrispondente
	return allocator->memory + offset;
}
		

// initializza il buddy allocator
void init_buddy(BuddyAllocator *allocator, uint8_t *buffer, int num_levels, char *memory, int min_bucket_size)
{
	// Pulizia e inizializzazione dell'allocatore
	memset(allocator, 0, sizeof(BuddyAllocator));
	// Inizializzo l'allocatore
	allocator->num_levels = num_levels;
	allocator->memory = memory;
	allocator->min_bucket_size = min_bucket_size;
	// Calcola la dimensione della bitmap in base al numero di livelli
	// Se abbiamo n livelli avremo (2^n - 1) blocchi totali
	int num_bits = (1 << num_levels) - 1;
	// Inizializzo la bitmap con il buffer fornito e il numero totale di blocchi
	createBitMap(&allocator->bitmap, num_bits, buffer);
	// Imposto tutti i bit a 0 (libero)
	memset(allocator->bitmap.buffer, 0, allocator->bitmap.buffer_size);
	// All'inizio, l'intera memoria è un grande blocco libero 
	setBit(&allocator->bitmap, 0, 1);
	// Imposta tutti i blocchi come liberi
	//for (int i = 0; i < num_bits; i++) 
		//setBit(&allocator->bitmap, i, 1);
	printf("Bitmap inizializzata\n");
	bitmap_print(&allocator->bitmap);
	printf("Buddy Initializing\n");
	printf("\tlevels: %d", num_levels);
	printf("\tmin_bucket_size: %d bytes\n", min_bucket_size);
	printf("\tmanaged memory %d bytes\n", (1 << (num_levels - 1)) * min_bucket_size);
}

// Funzione che ritorna l'indice del buddy richiesto
int buddy_allocator_get_buddy(BuddyAllocator *allocator, int level) {
	int num_levels = allocator->num_levels - 1;
	// Verifica che il livello richiesto sia valido
	if (level < 0 || level > num_levels + 1) {
		printf("Livello non valido \n");
		return -1;
	}
	// Cerco il primo blocco libero nel livello
	int idx = find_first_free_block(allocator, level);
	// Se abbiamo trovato un blocco libero nel livello specificato
	if (idx != -1) {
		// Segna il blocco come occupato
		setBit(&allocator->bitmap, idx, 0);
		// Ritorna l'indice del buddy
		return idx;
	} else {
		// Se non abbiamo trovato un blocco libero, procediamo al livello superiore
		for (int l = level - 1; l >= 0; l--) {
			idx = find_first_free_block(allocator, l);
			if (idx != -1) {
				// Se abbiamo trovato un blocco libero ad un livello superiore
				int left_child_idx = idx * 2 + 1;
				int right_child_idx = left_child_idx + 1;
				// Segna il figlio destro come disponibile 
				setBit(&allocator->bitmap, right_child_idx, 1);
				// Segna il figlio sinistro come occupato
				setBit(&allocator->bitmap, left_child_idx, 0);
				// Ritorna l'indice del figlio sinistro
				return left_child_idx;
			}
		}
	}
	// Stampo la bitmap
	bitmap_print(&allocator->bitmap);
	printf("Nessun blocco libero trovato\n");
	// Se siamo arrivati a questo punto non ci sono blocchi disponibili
	return -1;
}				

// Funzione che rilascia il buddy e lo unisce con il gemello
void BuddyAllocator_releaseBuddy(BuddyAllocator *allocator, int idx) {
	// Rilascia il blocco corrente
	setBit(&allocator->bitmap, idx, 1);
	// Se idx è 0, siamo alla radice
	while (idx > 0) {
		int buddy_idx = get_buddy_index(idx);
		// Se il buddy non è libero, interrompi
		if (!is_valid_index(buddy_idx, allocator->bitmap.num_bits) || getBit(&allocator->bitmap, buddy_idx) == 0) {
			break;
		}
		// Altrimenti rilascio il blocco padre
		int parent_idx = parentIdx(idx);
		// Imposta come non assegnato il padre
		setBit(&allocator->bitmap, parent_idx, 1);
		setBit(&allocator->bitmap, idx, 0);
		setBit(&allocator->bitmap, buddy_idx, 0);
		// Passa al livello superiore per continuare la verifica
		idx = parent_idx;
	}
	// Stampo la bitmap
	bitmap_print(&allocator->bitmap);
}

void *BuddyAllocator_malloc(BuddyAllocator *allocator, int size) {
	if (!allocator || size <= 0) {
		printf("Errore: allocazione non riuscita. Allocator non valido o dimensione non valida\n");
		return NULL; 
	}
	printf("Richiesta di allocazione di %d bytes\n", size);
	// Determina il livello corretto per la dimensione richiesta
	int level = get_level(allocator, size);
	printf("Livello ottimale per l'allocazione: %d\n", level);
	// Cerca un blocco libero al livello appropriato
	int idx = -1;
	for (int l = level; l >= 0; l--) {
		idx = buddy_allocator_get_buddy(allocator, l);
		if (idx != -1) {
			printf("Blocco trovato al livello %d con indice %d \n", l, idx);
			// Blocco trovato
			break;
		} else {
			printf("Nessun blocco disponibile al livello %d, passando al livello superiore\n", l);
		}
	}
	// Se non viene trovato un blocco libero, ritorna NULL
	if (idx == -1) {
		printf("Errore: allocazione non riuscita, Nessun blocco disponibile\n");
		return NULL;
	}
	// Stampo la bitmap
	bitmap_print(&allocator->bitmap);
	// Restituisci l'indirizzo di memoria corrispondente
	void* address = get_memory(allocator, level, idx);
	printf("Memoria allocata all'indirizzo: %p\n", address);
	return address;
}

void BuddyAllocator_free(BuddyAllocator *allocator, void *mem) {
	if (!allocator || !mem) {
		printf("Errore: Allocator o indirizzo di memoria non validi\n");
		return;
	}
	// Calcola l'offset della base della memoria
	// Distanza dell'indirizzo mem rispetto alla memoria gestita dal Buddy Allocator
	int offset = (char*)mem - allocator->memory;
	// Determina la dimensione del blocco basata sull'offset
	int current_size = allocator->min_bucket_size;
	int level;
	// Determina il livello controllando per quale dimensione di blocco l'offset è un multiplo esatto
	for (level = allocator->num_levels - 1; level >= 0; level--) {
		if (offset % current_size == 0) {
			break;
		}
		current_size *= 2;
	}
	// Calcola l'indice del blocco basato sull'offset e sul livello
	int base_idx = (1 << level) -1; // indice iniziale per il livello
	int idx = base_idx + offset / current_size;
	printf("Liberando il blocco con indice %d al livello %d\n", idx, level);
	// Rilascio il blocco di memoria
	BuddyAllocator_releaseBuddy(allocator, idx);
	// Stampo la bitmap
	bitmap_print(&allocator->bitmap);
}

/*void BuddyAllocator_free(BuddyAllocator *allocator, void *mem)
{
	if (mem == NULL)
	{
		printf("Memoria nulla. Impossibile deallocare\n");
		return;
	}
	int idx = *((int*)mem - 1);
	// Verifica che l'indice sia valido
	if (idx < 0 || idx > allocator->bitmap.num_bits)
	{
		printf("Indice non valido. Impossible deallocare\n");
		return;
	}
	// Rilascia il blocco nel buddy allocator
	BuddyAllocator_releaseBuddy(allocator, idx);
	printf("Blocco deallocato:\n");
	printf("\tIndice: %d\n", idx);
	printf("\tIndirizzo: %p\n", mem);
	printf("\tLivello: %d\n", allocator->num_levels - 1);
}



// Funzione che ritorna l'indice del buddy richiesto
int buddy_allocator_get_buddy(BuddyAllocator *allocator, int level)
{
	int num_levels = allocator->num_levels - 1;
	int min_bucket_size = allocator->min_bucket_size;
	// Verifica che il livello richiesto sia valido
	if (level < 0 || level > num_levels + 1)
	{
		printf("Livello non valido \n");
		return -1;
	}
	// Cerco il primo blocco libero nel livello
	int idx = find_first_free_block(allocator, level);
	if (idx == -1) {
		// Nessun blocco disponibile a questo livello cerco sopra
		int upper_buddy = buddy_allocator_get_buddy(allocator, level -1);
		// Nessun parent disponibile
		if (upper_buddy == -1) {
			printf("Nessun Blocco parent disponibile\n");
			return -1;
		}
		// Calcolo i 2 figli e lascio libero il buddy destro
		int left_child = upper_buddy * 2 + 1;
		int right_child = upper_buddy * 2 + 2;
		setBit(&allocator->bitmap, right_child, 1);
		idx = left_child;
	}
	// Imposto il buddy selezionato come non disponibile
	setBit(&allocator->bitmap, idx, 0);
	// Calcola l'indirizzo di memoria del blocco richiesto
	int start = (1 << level) -1;
	char *memory = allocator->memory;
	int block_size = (1 << (num_levels - level)) * min_bucket_size;
	int offset = (idx - start) * min_bucket_size;
	char *block_address = memory + offset;
	// Stampa per informazioni sul blocco assegnato
	printf("Blocco assegnato:\n");
	printf("\tLivello: %d\n", level);
	printf("\tDimensione: %d bytes\n", block_size);
	printf("\tIndirizzo: %p\n", block_address);
	return idx;
}

void BuddyAllocator_releaseBuddy(BuddyAllocator *allocator, int idx)
{
	int num_levels = allocator->num_levels - 1;
	int min_bucket_size = allocator->min_bucket_size;
	// Verifica 	che l'indice sia valido
	if (idx < 0 || idx > (1 << num_levels) + 1)
	{
		printf("Indice non valido.\n");
		return;
	}
	// Calcolo l'indice del buddy fratello
	int buddy_idx = get_buddy_index(idx);
	int parent_idx = parentIdx(idx);	
	// Finchè il buddy fratello è libero ed il parent non è assegnato
	while (getBit(&allocator->bitmap, buddy_idx) && !getBit(&allocator->bitmap, parent_idx)) {
		// Imposta come non assegnato il padre
		setBit(&allocator->bitmap, parent_idx, 1);
		setBit(&allocator->bitmap, idx, 0);
		setBit(&allocator->bitmap, buddy_idx, 0);
		// Passa al livello superiore
		idx = parent_idx;
		// Prendi i nuovi valori di buddy brother e parent
		buddy_idx = get_buddy_index(idx);
		parent_idx = parentIdx(idx);
	}
	// Calcolo il livello dell'indice richiesto
	int level = levelIdx(idx);
	// Stampa informazioni sul blocco rilasciato
	int start = (1 << level) -1;
	char *memory = allocator->memory;
	int block_size = (1 << (num_levels - level)) * min_bucket_size;
	int offset = (idx - start) * min_bucket_size;
	char *block_address = memory + offset;
	printf("Blocco rilasciato:\n");
	printf("\tIndice: %d\n", idx);
	printf("\tLivello: %d\n", levelIdx(idx));
	printf("\tDimensione: %d bytes\n", block_size);
	printf("\tIndirizzo: %p\n", block_address);
}

void *BuddyAllocator_malloc(BuddyAllocator *allocator, int size)
{
	int num_levels = allocator->num_levels -1;
	char *memory = allocator->memory;
	int min_bucket_size = allocator->min_bucket_size;
	int max_block_size = (1 << num_levels) * min_bucket_size;
	size += sizeof(int);
	// int block_size = 1 << (num_levels - 1);
	// Verifica che la dimensione richiesta sia valida
	if (size > max_block_size)
	{
		// Allocazione Fallita
		printf("La dimensione richiesta supera la dimensione massima gestibile\n");
		return NULL;
	}
	// Cerca il livello appropriato che può contenere la dimensione richiesta
	int level = get_level(allocator, size);
	// Ottieni il blocco dal buddy allocator
	int idx = buddy_allocator_get_buddy(allocator, level);
	if (idx == -1) {
		// Allocazione Fallita
		printf("Impossibile trovare spazio necessario\n");
		return NULL;
	}
	// Calcola l'indirizzo di memoria del blocco assegnato
	int start = (1 << level) -1;
	int block_size = min_bucket_size * (1 << (num_levels - level));
	int offset = (idx - start) * min_bucket_size;
	char *block_address = memory + offset;
	int *address = (int*)get_memory(allocator, level, idx);
	*address = idx;
	// Restituisce il puntatore al blocco di memoria ottenuto dal buddy allocator
	printf("Blocco allocato dal buddy allocator\n");
	printf("\tLivello: %d\n", level);
	printf("\tDimensione: %d bytes\n", block_size);
	printf("\tIndirizzo: %p\n", address);
	printf("\tIndice: %d\n", idx);
	return address + 1;
}*/
