#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
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

// Funzione ausiliaria che azzera tutti i bit della bitmap
void clearAllBits(Bitmap* bitmap) {
	for (int i = 0; i < bitmap->buffer_size; i++) {
		bitmap->buffer[i] = 0;
	}
}

// Funzione che dato il livello cerca il primo blocco libero
int find_first_free_block(const Bitmap *bitmap, int level) {
	//assert(level < allocator->num_levels);
	int start_idx = (1 << level) - 1;
	int end_idx = (1 << (level + 1)) -1;
	for (int idx = start_idx; idx < end_idx; idx++) {
		// Blocco libero trovato ritorno l'indice
		if (getBit(bitmap, idx)) {
			return idx;
		}
	}
	// Nessun blocco libero
	return -1;
}

// Funzione che ritorna il livello del più piccolo blocco che può contenere i bytes
int get_level(BuddyAllocator *allocator, int size) {
	if (size <= 0) {
		// Dimensione non valida
		return -1;
	}
	// Se la size è già più piccola
	if (size <= allocator->min_bucket_size) {
		return allocator->num_levels - 1;
	}
	int level = 0;
	int block_size = allocator->min_bucket_size;
	while (block_size < size) {
		block_size *= 2;
		level++;
	}
	int res = allocator->num_levels- level - 1;
	return res;
}

// Funzione che cerca il buddy 
int find_and_divide(BuddyAllocator* allocator, int level) {
	// Se raggiungiamo il livello -1, significa che non abbiamo trovato nessun blocco libero disponibile
	if (level < 0) {
		return -1;
	}
	// Cerco un blocco libero nel livello corrente
	int idx = find_first_free_block(&allocator->bitmap, level);
	// Se abbiamo trovato un blocco libero nel livello specificato
	if (idx != -1) {
		// Imposto il blocco come assegnato
		setBit(&allocator->bitmap, idx, 0);
		// Ritorno l'indice del blocco
		return idx;
	}
	// Se non abbiamo trovato un blocco libero nel livello specificato
	// Provo a cercare un blocco ad un livello superiore
	int parent_idx = find_and_divide(allocator, level - 1);
	// Se ho trovato un blocco nel livello superiore
	if (parent_idx != -1) {
		// Imposto il blocco parent come assegnato
		setBit(&allocator->bitmap, parent_idx, 0);
		// Calcolo l'indice del figlio sinistro
		int left_idx = parent_idx * 2 + 1;
		// Calcolo l'indice del figlio destro
		int right_idx = left_idx + 1;
		// Divido il blocco
		setBit(&allocator->bitmap, right_idx, 1);
		setBit(&allocator->bitmap, left_idx, 0);
		return left_idx;
	}
	// Se tutti i blocchi sono occupati
	return -1;		
}


// Funzione che ritorna l'indirizzo di memoria del buddy
BuddyBlockHeader* get_memory(BuddyAllocator *allocator, int level, int idx) {
	if (!allocator || !allocator->memory) {
		printf("Errore: allocator non valido o memoria non inizializzata\n");
		return NULL;
	}
	// Verifica che il livello e l'indice siano validi
	if (level < 0 || level >= allocator->num_levels || idx < 0 || idx >= (1 << (level + 1))) {
		printf("Errore: livello o indice non validi\n");
		return NULL;
	}
	// Calcola la dimensione del blocco per il livello specificato
	size_t block_size = allocator->min_bucket_size * (1 << (allocator->num_levels - level - 1));
	// Calcola l'indice di inizio per il livello specificato
	size_t start_idx = (1 << level) -1;
	// Calcola l'offset in memoria basato sull'indice del blocco
	size_t offset = (idx - start_idx) * block_size;
	// Ritorna l'indirizzo di memoria corrispondente come BuddyBlockHeader
	return (BuddyBlockHeader*)(allocator->memory + offset);
}

// Funzione che stampa a schermo l'indice del blocco
void print_metadata(void* mem) {
	if (mem == NULL) {
		printf("Errore: indirizzo di memoria non valido\n");
		return;
	}
	// Ottieni i metadati spostandoti indietro dalla memoria fornita
	BuddyBlockHeader* metadata = ((BuddyBlockHeader*)mem - 1);
	// Stampa i dettagli dei metadati
	printf("Metadati del blocco di memoria:\n");
	printf("Indirizzo di memoria: %p\n", metadata);
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

// inizializza il buddy allocator
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
	clearAllBits(&allocator->bitmap);
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
	// Cerco un buddy libero
	int idx = find_and_divide(allocator, level);
	if (idx == -1) {
		printf("Nessun blocco libero trovato\n");
		return idx;
	}
	// Ritorno l'indice del buddy
	return idx;
}			

// Funzione che rilascia il buddy e lo unisce con il gemello
void BuddyAllocator_releaseBuddy(BuddyAllocator *allocator, int idx) {
	// Se idx è 0, siamo alla radice
	while (true) {
		// Calcolo il blocco gemello
		int buddy_idx = get_buddy_index(idx);
		// Calcolo il blocco parent
		int parent_idx = parentIdx(idx);
		// Se il buddy non è libero, interrompi
		if (!getBit(&allocator->bitmap, buddy_idx)) {
			// Segno il blocco come disponibile
			setBit(&allocator->bitmap, idx, 1);
			break;
		}
		else {
			// Se il buddy è libero unisco i 2 blocchi
			setBit(&allocator->bitmap, idx, 0);
			setBit(&allocator->bitmap, buddy_idx, 0);
			setBit(&allocator->bitmap, parent_idx, 1);
			// Imposto l'indice a quello del padre e continuo con il livello superiore
			idx = parent_idx;
		}
	}
}

// Funzione per la malloc
void *BuddyAllocator_malloc(BuddyAllocator *allocator, int size) {
	if (!allocator || size <= 0) {
		printf("Errore: allocazione non riuscita. Allocator non valido o dimensione non valida\n");
		return NULL; 
	}
	// Aggiungo alla size richiesta i byte per salvare l'indice nella struct
	int total_size = size + sizeof(BuddyBlockHeader);
	printf("Richiesta di allocazione di %d bytes\n", size);
	// Determina il livello corretto per la dimensione richiesta
	int level = get_level(allocator, total_size);
	printf("Livello ottimale per l'allocazione: %d\n", level);
	// Cerca un blocco libero al livello appropriato
	//int idx = buddy_allocator_get_buddy(allocator, level);
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
	BuddyBlockHeader* header = (BuddyBlockHeader*)get_memory(allocator, level, idx);
	if (header == NULL) {
		printf("Errore nella get memory\n");
		return NULL;
	}
	// Salva l'indice del buddy nei metadati
	header->buddy_index = idx;
	// Stampo la bitmap
	bitmap_print(&allocator->bitmap);
	// Calcolo l'indirizzo di memoria da ritornare all'utente
	void* user_memory = (void*)(header + 1);
	// Stampa i metadati appena dopo averli assegnati
	print_metadata(user_memory);
	// Restituisco l'indirizzo appena dopo i metadati al chiamante
	return user_memory;
}

// Funzione per la free
void BuddyAllocator_free(BuddyAllocator *allocator, void *mem) {
	if (!allocator || !mem) {
		printf("Errore: Allocator o indirizzo di memoria non validi\n");
		return;
	}
	// Stampa i metadati prima di liberare la memoria
	print_metadata(mem);
	// Ottieni i metadati spostandoti indietro dalla memoria fornita
	BuddyBlockHeader *header = ((BuddyBlockHeader*)mem - 1);
	// Recupera l'indice del buddy dai metadati
	int idx = header->buddy_index;
	// Calcola il livello dell'indice
	int level = get_level_from_index(idx);
	printf("Liberando il blocco con indice %d al livello %d\n", idx, level);
	// Rilascio il blocco di memoria
	BuddyAllocator_releaseBuddy(allocator, idx);
	// Stampa la bitmap
	bitmap_print(&allocator->bitmap);
}


	
