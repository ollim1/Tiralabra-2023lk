#ifndef LZSS_BYTE_PRIVATE_H
#define LZSS_BYTE_PRIVATE_H
#include "bitarray.h"
#include "ringbuffer.h"

void encodeLZSSPayloadByteLevel(Buffer *src, Buffer *dst);
Buffer *decodeLZSSPayloadByteLevel(BufferReader *reader);

int writeByteToken(Buffer *dst, unsigned distance, unsigned length);
int writeByteString(Buffer *dst, Buffer *src);
int readByteToken(BufferReader *src, unsigned *distance, unsigned *length);
#endif
