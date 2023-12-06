#ifndef LZSS_H
#define LZSS_H

#include "buffer.h"

Buffer *lzss_compress(Buffer *src);
Buffer *lzss_extract(Buffer *src);
#endif
