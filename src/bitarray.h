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
typedef struct bitarrayreader_st {
    BitArray *data;
    size_t pos;
} BitArrayReader;

BitArray *new_bitarray();
void delete_bitarray(BitArray *);
void bitarray_set(BitArray *, int, size_t);
BitArray *bitarray_copyl(BitArray *, size_t);
BitArray *new_bitarray_fromStringl(char *code, size_t maxLen);
Buffer *bitarray_toBuffer(BitArray *ba);
char *bitarray_toString(BitArray *ba);
BitArray *bitarray_fromBuffer(Buffer *src);
int bitarray_equals(BitArray *a, BitArray *b);
void bitarray_concat(BitArray *, BitArray *);
void bitarray_concatl(BitArray *a, BitArray *b, size_t len);
BitArrayReader *bitarray_createReader(BitArray *);
int bitarrayreader_readBit(BitArrayReader *br, int *dst);
int bitarrayreader_readByte(BitArrayReader *br, unsigned char *dst);
void delete_bitarrayreader(BitArrayReader *);
void bitarray_append(BitArray *, int);
void bitarray_appendByte(BitArray *, unsigned char);
void bitarray_appendString(BitArray *, unsigned char *, size_t len);
void bitarray_setString(BitArray *, unsigned char *, size_t len, size_t pos);
void bitarray_pad(BitArray *, size_t);
int bitarray_get(BitArray *, size_t);
unsigned char bitarray_getByte(BitArray *, size_t);
void bitarray_writeInteger(BitArray *ba, size_t val);
size_t bitarrayreader_readInteger(BitArrayReader *br);

#endif
