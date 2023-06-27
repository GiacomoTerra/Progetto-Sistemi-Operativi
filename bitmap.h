#pragma once
#include <stdint.h>

#define BITMAP_SIZE_IN_BYTES 1024 * 1024 // 1 MB

typedef struct {
	uint8_t* buffer;
	int buffer_size;
	int num_bits;
} Bitmap;

void createBitMap(Bitmap* BitMap, int num_bits, uint8_t* buffer);
void destroyBitMap(Bitmap* BitMap);
void setBit(Bitmap* BitMap, int i, int stato);
int getBit(Bitmap* BitMap, int i);
void clearBit(Bitmap* BitMap, int i);
int getBytes(int bits);
int BitMap_bit(const Bitmap* BitMap, int bit_num);
void BitMap_print(Bitmap* BitMap);
int testbit(Bitmap* bitmap, int index);
int bitmap_test();
