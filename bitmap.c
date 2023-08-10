#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "bitmap.h"

//ritorna il numero dei bytes per memorizzare tutti i bits
int getBytes(int num_bits) {
	return num_bits / 8 + (num_bits % 8) != 0; 
}

//inizializza una bitmap su un array esterno
void createBitMap(Bitmap* BitMap, int num_bits, uint8_t* buffer) {
	BitMap->buffer = buffer;
	BitMap->num_bits = num_bits;
	BitMap->buffer_size = getBytes(num_bits);
}

//distrugge una bitmap
void destroyBitMap(Bitmap* BitMap) {
	if (BitMap == NULL) return;
	// free(BitMap->buffer);
	free(BitMap);
}

//setta il bit in posizione i, stato 0/1
void setBit(Bitmap* bitmap, int index, int state) {
	if (index < 0 || index >= bitmap->num_bits) {
		printf("Indice del bit fuori dal range della bitmap\n");
		return;
	}
	int byte_num = index >> 3;
	int bit_in_byte = byte_num&0x03;
	if (state) {
		bitmap->buffer[byte_num] |= (1<<bit_in_byte);
	} else {
		bitmap->buffer[byte_num] &= ~(1<<bit_in_byte);
	}
}

//ritorna il bit in posizione i
int getBit(Bitmap* BitMap, int i) {
	if (BitMap == NULL || BitMap->num_bits <= i) return 0;
	int byte_index = i / 8;
	int bit_offset = i % 8;
	return (BitMap->buffer[byte_index] >> bit_offset) & 1;
}

//inspeziona lo stato del bit in posizione i
int BitMap_bit(const Bitmap* BitMap, int bit_num) {
	int byte_num = bit_num >> 3;
	assert(byte_num < BitMap->buffer_size);
	int bit_in_byte = byte_num&0x03;
	return (BitMap->buffer[byte_num] & (1 << bit_in_byte)) != 0;
}

//stampa la bitmap a schermo
void BitMap_print(Bitmap* BitMap) {
	uint8_t byte;
	int num_bytes = (BitMap->num_bits + 7) / 8;
	for (int i = 0; i < num_bytes; i++) {
		byte = BitMap->buffer[i];
		for (int j = 0; j < 8 && (i *8 + j) < BitMap->num_bits; j++) {
			if (byte & (1 << j)) {
				printf("1");
			} else {
				printf("0");
			}
		}
		printf(" ");
	}
	printf("\n");
}

// verifica lo stato di un bit specifico all'interno di una bitmap
// restituisce il valore del bit corrispondente all'indice nella bitmap
int testbit(Bitmap* bitmap, int index) {
	int byte_index = index / 8;
	int bit_offset = index % 8;
	uint8_t byte = bitmap->buffer[byte_index];
	return (byte >> bit_offset) & 0x1;
}
	
	
	
	
	
	
