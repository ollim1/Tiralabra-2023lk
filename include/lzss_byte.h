#ifndef LZSS_BYTE_H
#define LZSS_BYTE_H

#include "buffer.h"

Buffer *lzss_byte_compress(Buffer *src);
Buffer *lzss_byte_extract(Buffer *src);
#endif

