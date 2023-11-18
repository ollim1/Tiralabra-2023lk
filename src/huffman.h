#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "buffer.h"
#include "tree.h"
#include <string.h>
#include <sys/types.h>

Buffer *huffman_compress(Buffer *src);
Buffer *huffman_extract(Buffer *src);
void encodeLength(BitArray *ba, size_t val);
size_t decodeLength(BitArrayReader *br);
HuffNode *buildHufftree(Buffer *src);
void cacheHuffcodes(HuffNode *, BitArray **, char *, int length);
void encodePayload(Buffer *src, BitArray *dst, BitArray **codes);
Buffer *decodePayload(BitArrayReader *reader, HuffNode *tree, size_t decoded_length);
#endif
