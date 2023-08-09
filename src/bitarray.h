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

BitArray *new_bitarray();
void delete_bitarray(BitArray *);
void bitarray_set(BitArray *, int, size_t);
void bitarray_append(BitArray *, int);
void bitarray_appendString(BitArray *, char *, size_t len);
void bitarray_pad(BitArray *, size_t);
int bitarray_get(BitArray *, size_t);
#endif
