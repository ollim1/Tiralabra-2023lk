#include "huffman.h"

HuffNode *buildHufftree(Buffer *src);
void encodeLength(BitArray *ba, size_t val);
size_t decodeLength(BitArrayReader *br);
HuffNode *buildHufftree(Buffer *src);
void cacheHuffcodes(HuffNode *, BitArray **, char *, int length);
void encodeHuffmanPayload(Buffer *src, BitArray *dst, BitArray **codes);
Buffer *decodeHuffmanPayload(BitArrayReader *reader, HuffNode *tree, size_t decoded_length);
