#ifndef LZSS_COMMON_H
#define LZSS_COMMON_H

#include "bitarray.h"
#include "ringbuffer.h"
#define WINDOW_SIZE 4096 // size of dictionary window
#define TOKEN_DISTANCE_BITS 12 // amount of bits for distance of reference from current position (max value 4095
#define TOKEN_LENGTH_BITS 4 // amount of bits for length of reference (max value 15)
#define TOKEN_MAXLEN 15

int findMatchKMP(RingBuffer *haystack, Buffer *needle);
void genKMPTable(Buffer *needle, int *table);

#endif
