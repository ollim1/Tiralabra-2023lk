#ifndef CODEDTREE_H
#define CODEDTREE_H

#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "bitarray.h"
#include "error.h"
#include "queue.h"
/*
 * Functions for serializing and deserializing a Huffman tree
 * consists of 256 8-bit values for lengths of Huffman tree traversal codes,
 * followed by Huffman tree traversal codes
 */

typedef struct huffcode_st {
    char value;
    char code[256]; // 256 bits of traversal path written out as bits
    char len;
} HuffCode;

BitArray *serialize_hufftree(HuffNode *src); // takes a Huffman tree node, traverses depth-first and returns a bit array
unsigned deserialize_hufftree(BitArray *src, HuffNode *dst); // forms a Huffman tree at dst from data at src, returns length of serialized tree in bits
int huffcode_compare(void *a, void *b);

#endif
