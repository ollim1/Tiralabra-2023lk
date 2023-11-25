#ifndef LZSS_PRIVATE_H
#define LZSS_PRIVATE_H
#include "bitarray.h"
#define WINDOW_SIZE 4096 // size of dictionary window
#define TOKEN_POS_BITS 12 // amount of bits for position in dictionary window
#define TOKEN_DIST_BITS 4 // amount of bits for reference string

typedef struct lztoken_st {
    int exists;
    int pos;
    int dist;
} LZToken;

void writeToken(BitArray *dst, LZToken *token);
int readToken(BitArrayReader *src, LZToken *dst);
#endif
