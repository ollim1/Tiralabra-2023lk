#ifndef BITARRAY_H
#define BITARRAY_H

#include "buffer.h"

/*
 * Bit array struct
 * Implements an array with bitwise access
 */
typedef struct bitarray_st {
    Buffer *data;
    size_t len;
} BitArray;
typedef struct bitwriter_st {
    BitArray *data;
    size_t pos;
} BitArrayReader;

BitArray *new_bitarray();
void delete_bitarray(BitArray *);
void bitarray_set(BitArray *, int, size_t);
BitArrayReader *bitarray_createReader(BitArray *);
int bitarrayreader_readBit(BitArrayReader *br, int *dst);
int bitarrayreader_readByte(BitArrayReader *br, char *dst);
void delete_bitarrayreader(BitArrayReader *);
void bitarray_append(BitArray *, int);
void bitarray_appendByte(BitArray *, char);
void bitarray_appendString(BitArray *, char *, size_t len);
void bitarray_setString(BitArray *, char *, size_t len, size_t pos);
void bitarray_pad(BitArray *, size_t);
int bitarray_get(BitArray *, size_t);
char bitarray_getByte(BitArray *, size_t);
#endif