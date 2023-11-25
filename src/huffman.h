#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "buffer.h"
#include "huffnode.h"
#include <string.h>
#include <sys/types.h>
#define MAX_LEAVES 256 // number of possible 8-bit values

Buffer *huffman_compress(Buffer *src);
Buffer *huffman_extract(Buffer *src);
#endif
