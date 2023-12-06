#ifndef LZSS_PRIVATE_H
#define LZSS_PRIVATE_H
#include "bitarray.h"
#include "ringbuffer.h"
#define WINDOW_SIZE 4096 // size of dictionary window
#define TOKEN_DISTANCE_BITS 12 // amount of bits for distance of reference from current position (max value 4095
#define TOKEN_LENGTH_BITS 4 // amount of bits for length of reference (max value 15)

int findMatch(RingBuffer *haystack, Buffer *needle);
void writeToken(BitArray *dst, unsigned distance, unsigned length);
int readToken(BitArrayReader *src, unsigned *distance, unsigned *length);
void writeString(BitArray *dst, Buffer *src);
void encodeLZSSPayloadBitLevel(Buffer *src, BitArray *dst);
Buffer *decodeLZSSPayloadBitLevel(BitArrayReader *reader, size_t decoded_length);
#endif
