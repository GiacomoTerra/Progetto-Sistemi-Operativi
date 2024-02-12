#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bitmap.h"

//ritorna il numero dei bytes per memorizzare tutti i bits
int getBytes(int num_bits) {
	// Restituisce la somma dei bytes che possono contenere i bits più il resto in caso ci sia bisogno
	return (num_bits / 8) + (num_bits % 8 ? 1 : 0); 
}

//inizializza una bitmap con un numero specificato di bit su un array esterno
void createBitMap(Bitmap* BitMap, int num_bits, uint8_t* buffer) {
	BitMap->buffer = buffer;
	BitMap->num_bits = num_bits;
	BitMap->buffer_size = getBytes(num_bits);
}

//distrugge una bitmap, liberando le risorse allocate
void destroyBitMap(Bitmap* BitMap) {
	if (BitMap == NULL) return;
	free(BitMap);
}

//Imposta un valore per il bit in posizione i, stato 0/1
void setBit(Bitmap* bitmap, int index, int state) {
	if (index < 0 || index >= bitmap->num_bits) {
		printf("Indice del bit fuori dal range della bitmap\n");
		return;
	}
	// Determina in quale byte si trova il bit, ogni byte contiene 8 bit
	int byte_num = index / 8;
	// Il resto indica la posizione del bit all'interno del byte
	int bit_in_byte = index % 8;
	if (state) {
		// Per impostare il bit a 1, si usa un'operazione OR bit a bit tra il byte corrente e un bitmask dove solo il bit di interesse è impostato a 1
		bitmap->buffer[byte_num] |= (1<<bit_in_byte);
	} else {
		// Per resettare il bit a 0, si usa un'operazione AND bit a bit (&=) con il complemento (~) di un bitmask simile al caso precedente
		// Il bitmask, ottenuto spostando a sinistra il valore 1 di bit_in_byte posizioni, viene complementato per avere 1 in tutte le posizioni tranne quella del bit di interesse
		bitmap->buffer[byte_num] &= ~(1<<bit_in_byte);
	}
}

//ritorna il bit in posizione i
int getBit(const Bitmap* BitMap, int i) {
	if (BitMap == NULL || i >= BitMap->num_bits || i < 0) return 0;
	int byte_index = i / 8;
	int bit_offset = i % 8;
	// sposta a destra il byte contenente il bit di bit_offset portando il bit nella posizione meno significativa e a questo punto si fa un AND con 1
	return (BitMap->buffer[byte_index] >> bit_offset) & 1;
}

//stampa la bitmap a schermo
void bitmap_print(Bitmap* bitmap) {
	if (!bitmap || !bitmap->buffer) {
		printf("Bitmap non valida o non inizializzata\n");
		return;
	}
	printf("Bitmap: ");
	for (int i = 0; i < bitmap->num_bits; i++) {
		// Aggiungere uno spazio ogni 8 bit
		if (i % 8 == 0 && i != 0) {
			printf(" ");
		}
		printf("%d", getBit(bitmap, i));
	}
	printf("\n");
}


	
	
	
	
	
	
