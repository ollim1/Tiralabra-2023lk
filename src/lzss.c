#include "lzss.h"
#include "ealloc.h"
#include "error.h"
#include "lzss_private.h"
#include "bitarray.h"
#include "ringbuffer.h"

Buffer *lzss_compress(Buffer *src)
{
    /*
     * Compresses Buffer using the LZSS (Lempel-Ziv-Storer-Szymanski algorithm.
     * Bit-level variant.
     */
    Buffer *ret = new_buffer();
    return ret;
}

BitArray *encodeLZSSPayload(Buffer *src)
{
    BitArray *ret = new_bitarray();
    RingBuffer *buf = new_ringbuffer(WINDOW_SIZE);
    for (size_t i = 0; i < src->len; i++) {
        ringbuffer_append(buf, src->data[i]);
    }
    return ret;
}

void writeToken(BitArray *dst, LZToken *src)
{
    /*
     * write LZSS reference token encoded in a 16-bit value
     */
    if (!dst || !src)
        err_quit("null pointer in writeToken");
    if ((src->pos >> TOKEN_POS_BITS) > 0)
        err_quit("invalid token reference position");
    if ((src->dist >> TOKEN_DIST_BITS) > 0)
        err_quit("invalid token reference distance");
    bitarray_append(dst, 1); // mark the beginning of a token
    int val = (src->pos << TOKEN_DIST_BITS) | src->dist;
    bitarray_appendByte(dst, val & 0xff);
    bitarray_appendByte(dst, val >> 8);
}

int readToken(BitArrayReader *src, LZToken *dst)
{
    /*
     * read LZSS reference token
     */
    if (!src || !dst)
        err_quit("null pointer in writeToken");
    int ret = 0;
    unsigned char temp;
    int val;
    ret += bitarrayreader_readByte(src, &temp);
    val = temp << 8;
    ret += bitarrayreader_readByte(src, &temp);
    val |= temp;
    dst->pos = val >> TOKEN_DIST_BITS;
    dst->dist = val & ((1 << TOKEN_DIST_BITS) - 1);
    return ret;
}

Buffer *lzss_extract(Buffer *src);
