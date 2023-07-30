#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "buffer.h"
#include <string.h>

Buffer *huffman_compress(Buffer *src);
Buffer *huffman_extract(Buffer *src);
#endif
