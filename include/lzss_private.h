#ifndef LZSS_PRIVATE_H
#define LZSS_PRIVATE_H
#include "bitarray.h"
#include "ringbuffer.h"

void writeToken(BitArray *dst, unsigned distance, unsigned length);
int readToken(BitArrayReader *src, unsigned *distance, unsigned *length);
void writeString(BitArray *dst, Buffer *src);
void encodeLZSSPayloadBitLevel(Buffer *src, BitArray *dst);
Buffer *decodeLZSSPayloadBitLevel(BitArrayReader *reader, size_t decoded_length);
#endif
