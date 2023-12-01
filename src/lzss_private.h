#ifndef LZSS_PRIVATE_H
#define LZSS_PRIVATE_H
#include "bitarray.h"
#include "ringbuffer.h"
#define WINDOW_SIZE 4096 // size of dictionary window
#define TOKEN_DISTANCE_BITS 12 // amount of bits for position in dictionary window
#define TOKEN_LENGTH_BITS 4 // amount of bits for reference string

int findMatch(RingBuffer *haystack, Buffer *needle);
void writeToken(BitArray *dst, int distance, int length);
int readToken(BitArrayReader *src, int *distance, int *length);
void writeString(BitArray *dst, Buffer *src);
void encodeLZSSPayloadBitLevel(Buffer *src, BitArray *dst);
Buffer *decodeLZSSPayloadBitLevel(BitArrayReader *reader, size_t decoded_length);
#endif
