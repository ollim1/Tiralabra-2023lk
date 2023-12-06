#ifndef LZSS_BYTE_PRIVATE_H
#define LZSS_BYTE_PRIVATE_H
#include "bitarray.h"
#include "ringbuffer.h"
#define WINDOW_SIZE 4096 // size of dictionary window
#define TOKEN_DISTANCE_BITS 12 // amount of bits for distance of reference from current position (max value 4095
#define TOKEN_LENGTH_BITS 4 // amount of bits for length of reference (max value 15)

int findMatch(RingBuffer *haystack, Buffer *needle);
void encodeLZSSPayloadByteLevel(Buffer *src, Buffer *dst);
Buffer *decodeLZSSPayloadByteLevel(BufferReader *reader);

int writeByteToken(Buffer *dst, unsigned distance, unsigned length);
int writeByteString(Buffer *dst, Buffer *src);
int readByteToken(BufferReader *src, unsigned *distance, unsigned *length);
#endif
