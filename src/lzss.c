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

void encodeLZSSPayloadBitLevel(Buffer *src, BitArray *dst)
{
    /*
     * encode LZSS payload. Bit-level implementation.
     * May implement a byte-level version in which every 9th byte contains the
     * start bits of the next 8 bytes. This may make it easier to compress using
     * the Huffman algorithm, at the cost of an extra bit used per reference token.
     */
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
                writeToken(dst, distance, length);
            } else {
                writeString(dst, lookahead);
            }
            buffer_clear(lookahead);
            buffer_clear(lookaheadNext);
        }
        // append to dictionary buffer
        buffer_append(lookahead, &c, 1);
        ringbuffer_append(dictionary, c);
    }
    delete_buffer(lookahead);
    delete_buffer(lookaheadNext);
    delete_ringbuffer(dictionary);
}

Buffer *decodeLZSSPayloadBitLevel(BitArrayReader *reader, size_t decoded_length)
{
    if (!reader)
        err_quit("null pointer when decoding LZSS payload");

    size_t total = 0;
    Buffer *output = new_buffer();
    while (total < decoded_length) {
        int bit = 0;

        if (bitarrayreader_readBit(reader, &bit) < 1)
            err_quit("unexpected end of file while reading payload");

        if (bit == 1) {
            // token bit set, the next two bytes will contain a token
            int distance = 0;
            int length = 0;
            if (readToken(reader, &distance, &length) < 16)
                err_quit("unexpected end of file while reading payload");
            // copy string indicated by token
            if (distance > output->len || distance <= 0 || distance < length)
                err_quit("token index out of bounds");
            buffer_append(output, &output->data[output->len - distance], length);
            total += length;
        } else {
            // token bit unset, next byte will be a literal
            unsigned char byte = 0;
            if (bitarrayreader_readByte(reader, &byte) < 8)
                err_quit("unexpected end of file while reading payload");
            buffer_append(output, &byte, 1);
            total++;
        }
    }

    return output;
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
            return distance + 1;
    }
    return -1;
}

void writeString(BitArray *dst, Buffer *src)
{
    /*
     * write string of LZSS literals to BitArray
     */
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
