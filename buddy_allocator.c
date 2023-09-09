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

// Funzione ausiliaria che ritorna l'indice buddy
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
		if (getBit(&allocator->bitmap, idx) == 1) {
			return idx;
		}
	}
	// Nessun blocco libero
	return -1;
}

// Funzione che ritorna il livello del più piccolo blocco che può contenere i bytes
int get_level(BuddyAllocator *allocator, int size) {
	if (size <= allocator->min_bucket_size) {
		return allocator->num_levels - 1;
	}
	int level = (int) log2(size);
	int target_level = allocator->num_levels - level;
	// Se la dimensione non è una potenza esatta di 2
	// ci vorrà un livello in meno per adattarla
	if (size != (1 << level)) {
		target_level--;
	}
	// Assicurazioni che il livello target non sia mai inferiore a 0
	return target_level < 0 ? 0 : target_level;
}

// Funzione che ritorna l'indirizzo di memoria del buddy
void *get_memory(BuddyAllocator *allocator, int level, int idx) {
	// Verifica che il livello e l'indice siano validi
	if (level < 0 || level > allocator->num_levels || idx < 0 || idx > (1 << level)) {
		printf("Errore: livello o indice non validi\n");
		return NULL;
	}
	// Calcola l'offset in memoria basato sull'indice del blocco
	int offset = (idx - ((1 << level) - 1)) * (allocator->min_bucket_size << level);
	// Ritorna l'indirizzo di memoria corrispondente
	return allocator->memory + offset;
}

// Funzione che stampa a schermo l'indice del blocco
void print_metadata(void* mem) {
	if (mem == NULL) {
		printf("Errore: indirizzo di memoria non valido\n");
		return;
	}
	// Ottieni i metadati spostandoti indietro dalla memoria fornita
	MemoryBlockMetadata* metadata = ((MemoryBlockMetadata*)mem) - 1;
	// Stampa i dettagli dei metadati
	printf("Metadati del blocco di memoria:\n");
	printf("Indice del buddy: %d\n", metadata->buddy_index);
}

// Funzione che dato l'indice dovrebbe tornare il livello
int get_level_from_index(int idx) {
	int level = 0;
	while (idx >= (1 << (level + 1)) - 1) {
		level++;
	}
	return level;
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
	for (int i = 0; i < num_bits; i++) 
		setBit(&allocator->bitmap, i, 1);
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
				setBit(&allocator->bitmap, idx, 0);
				// Ritorna l'indice del figlio sinistro
				return idx;
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
		if (!is_valid_index(buddy_idx, allocator->bitmap.num_bits) || getBit(&allocator->bitmap, buddy_idx) == 1) {
			break;
		}
		// Altrimenti rilascio il blocco padre
		int parent_idx = parentIdx(idx);
		// Imposta come non assegnato il padre
		setBit(&allocator->bitmap, parent_idx, 1);
		setBit(&allocator->bitmap, idx, 1);
		setBit(&allocator->bitmap, buddy_idx, 1);
		// Passa al livello superiore per continuare la verifica
		idx = parent_idx;
	}
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
	// Ottieni un puntatore alla memoria allocata
	MemoryBlockMetadata* metadata = get_memory(allocator, level, idx);
	if (metadata == NULL) {
		printf("Errore nella get memory\n");
		return NULL;
	}
	// Salva l'indice del buddy nei metadati
	metadata->buddy_index = idx;
	// Stampo la bitmap
	bitmap_print(&allocator->bitmap);
	void* user_memory = (void*)(metadata + 1);
	// Stampa i metadati appena dopo averli assegnati
	print_metadata(user_memory);
	// Restituisco l'indirizzo appena dopo i metadati al chiamante
	return user_memory;
}

void BuddyAllocator_free(BuddyAllocator *allocator, void *mem) {
	if (!allocator || !mem) {
		printf("Errore: Allocator o indirizzo di memoria non validi\n");
		return;
	}
	// Stampa i metadati prima di liberare la memoria
	print_metadata(mem);
	// Ottieni i metadati spostandoti indietro dalla memoria fornita
	MemoryBlockMetadata* metadata = ((MemoryBlockMetadata*)mem) - 1;
	// Recupera l'indice del buddy dai metadati
	int idx = metadata->buddy_index;
	// Calcola il livello dell'indice
	int level = get_level_from_index(idx);
	printf("Liberando il blocco con indice %d al livello %d\n", idx, level);
	// Rilascio il blocco di memoria
	BuddyAllocator_releaseBuddy(allocator, idx);
	// Stampa la bitmap
	bitmap_print(&allocator->bitmap);
}
