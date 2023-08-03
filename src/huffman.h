#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "buffer.h"
#include "tree.h"
#include <string.h>
#include <sys/types.h>
typedef struct huffbuf_st {
    HuffTreeNode *tree;
    Buffer *data;
} HuffBuffer;

Buffer *huffman_compress(Buffer *src);
Buffer *huffman_extract(Buffer *src);
#endif
