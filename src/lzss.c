#include "lzss.h"
#include "ealloc.h"
#include "error.h"
#include "lzss_private.h"
#include "bitarray.h"
#include "ringbuffer.h"
#include "buffer.h"

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
    RingBuffer *dictionary = new_ringbuffer(WINDOW_SIZE);
    Buffer *lookahead = new_buffer();
    Buffer *lookaheadNext = new_buffer();
    for (size_t i = 0; i < src->len; i++) {
        // read byte from input
        unsigned char c = src->data[i];
        // append to search buffer
        buffer_append(lookaheadNext, &c, 1);
        int indexNext = findMatch(dictionary, lookaheadNext);
        if (indexNext == -1 || i == src->len - 1) {
            if (i == src->len - 1 && indexNext >= 0)
                buffer_append(lookahead, &c, 1);

            if (lookahead->len > 1) {
                int index = findMatch(dictionary, lookahead);
                int distance =  index;
                int length = lookahead->len;
                writeToken(ret, distance, length);
            } else {
                writeString(ret, lookahead);
            }
            buffer_clear(lookahead);
            buffer_clear(lookaheadNext);
        }
        // append to dictionary buffer
        buffer_append(lookahead, &c, 1);
        ringbuffer_append(dictionary, c);
    }
    return ret;
}

int findMatch(RingBuffer *haystack, Buffer *needle)
{
    /*
     * find string needle from ringbuffer haystack in reverse, return distance
     * from buffer end
     * length 
     */

    int distance;
    int length = 0;
    for (distance = 0; distance < haystack->len; distance++) {
        unsigned char c = ringbuffer_getRev(haystack, distance);
        if (c == needle->data[needle->len - length - 1])
            length++;
        else
            length = 0;
        if (length >= needle->len)
            return distance;
    }
    return -1;
}

void writeString(BitArray *dst, Buffer *src)
{
    if (!dst || !src)
        err_quit("null pointer writing literal");
    for (size_t i = 0; i < src->len; i++) {
        bitarray_append(dst, 0);
        bitarray_appendByte(dst, src->data[i]);
    }
}

void writeToken(BitArray *dst, int distance, int length)
{
    /*
     * write LZSS reference token encoded in a 16-bit value
     */
    if (!dst)
        err_quit("null pointer in writeToken");
    if ((distance >> TOKEN_DISTANCE_BITS) > 0)
        err_quit("invalid token reference position");
    if ((length >> TOKEN_LENGTH_BITS) > 0)
        err_quit("invalid token reference distance");
    bitarray_append(dst, 1); // mark the beginning of a token
    int val = (distance << TOKEN_LENGTH_BITS) | length;
    bitarray_appendByte(dst, val & 0xff);
    bitarray_appendByte(dst, val >> 8);
}

int readToken(BitArrayReader *src, int *distance, int *length)
{
    /*
     * read LZSS reference token
     */
    if (!src)
        err_quit("null pointer in writeToken");
    int ret = 0;
    unsigned char temp;
    int val;
    ret += bitarrayreader_readByte(src, &temp);
    val = temp << 8;
    ret += bitarrayreader_readByte(src, &temp);
    val |= temp;
    *distance = val >> TOKEN_LENGTH_BITS;
    *length = val & ((1 << TOKEN_LENGTH_BITS) - 1);
    return ret;
}

Buffer *lzss_extract(Buffer *src);
