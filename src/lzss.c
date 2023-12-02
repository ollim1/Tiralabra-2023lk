#include "lzss.h"
#include "ealloc.h"
#include "error.h"
#include "lzss_private.h"
#include "bitarray.h"
#include "ringbuffer.h"
#include "buffer.h"
#include <stdint.h>

Buffer *lzss_compress(Buffer *src)
{
    /*
     * Compresses Buffer using the LZSS (Lempel-Ziv-Storer-Szymanski algorithm.
     * Bit-level variant.
     */

    if (!src)
        err_quit("null pointer in huffman_compress");
    if (src->len == 0) {
        fputs("file is empty, skipping compression\n", stderr);
        return src;
    }

    BitArray *compressed = new_bitarray();
    // write output length
    bitarray_writeInteger(compressed, src->len);
    encodeLZSSPayloadBitLevel(src, compressed);
    Buffer *ret = compressed->data;

    delete_bitarrayPreserveContents(compressed);
    return ret;
}

void encodeLZSSPayloadBitLevel(Buffer *src, BitArray *dst)
{
    /*
     * encode LZSS payload. Bit-level implementation. Each byte literal is prefixed
     * by a 'zero' bit, while two-byte tokens are prefixed by 'one' bits.
     * May implement a byte-level version in which every 9th byte contains the
     * start bits of the next 8 bytes. This may make it easier to compress using
     * the Huffman algorithm, at the cost of an extra bit used per reference token.
     */
    RingBuffer *dictionary = new_ringbuffer(WINDOW_SIZE);
    Buffer *search = new_buffer();
    Buffer *searchPrev = new_buffer();
    int searchIndexPrev = -1;
    size_t i;
    size_t symbols_output = 0;

    for (i = 0; i < src->len; i++) {
        // read byte from input
        unsigned char c = src->data[i];
        // append to search buffer
        buffer_append(search, &c, 1);
        int searchIndex = findMatch(dictionary, search);

        // if a matching string is in the ring buffer, then check if the
        // previous search string was
        if (searchIndex == -1 || i == src->len - 1 || searchPrev->len == 15) {
            if (i == src->len - 1 && searchIndex != -1) {
                buffer_append(searchPrev, &c, 1);
                searchIndexPrev = searchIndex;
            }

            if (searchPrev->len > 1) {
                int distance = searchIndexPrev - searchPrev->len;
                int length = searchPrev->len;
                writeToken(dst, distance, length);
            } else {
                writeString(dst, searchPrev);
            }
            symbols_output += searchPrev->len;
            // clear search buffers when done
            buffer_clear(searchPrev);
            buffer_truncate(search);
        }
        searchIndexPrev = searchIndex;
        buffer_append(searchPrev, &c, 1);
        // append to dictionary buffer
        ringbuffer_append(dictionary, c);
    }
    // if some symbols are still unencoded, append them to output
    if (symbols_output < i && search->len > 0)
        writeString(dst, search);

    delete_buffer(searchPrev);
    delete_buffer(search);
    delete_ringbuffer(dictionary);
}

Buffer *decodeLZSSPayloadBitLevel(BitArrayReader *reader, size_t decoded_length)
{
    if (!reader)
        err_quit("null pointer when decoding LZSS payload");

    Buffer *output = new_buffer();
    while (output->len < decoded_length) {
        int bit = 0;

        if (bitarrayreader_readBit(reader, &bit) < 1)
            err_quit("unexpected end of file while reading payload bit");

        if (bit == 1) {
            // token bit set, the next two bytes will contain a token
            unsigned distance = 0;
            unsigned length = 0;
            if (readToken(reader, &distance, &length) < 16)
                err_quit("unexpected end of file while reading payload token");
            // copy string indicated by token
            if (distance > output->len || distance < length)
                err_quit("token distance out of bounds");
            buffer_append(output, &output->data[output->len - distance - 1], length);
        } else {
            // token bit unset, next byte will be a literal
            unsigned char byte = 0;
            if (bitarrayreader_readByte(reader, &byte) < 8)
                err_quit("unexpected end of file while reading payload literal");
            buffer_append(output, &byte, 1);
        }
    }

    return output;
}

int findMatch(RingBuffer *haystack, Buffer *needle)
{
    /*
     * find string needle from ringbuffer haystack in reverse, return distance
     * from buffer end
     */

    if (!haystack || !needle)
        err_quit("null pointer in findMatch");

    if (needle->len < 1 || haystack->len < 1)
        return -1;
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

void writeToken(BitArray *dst, unsigned distance, unsigned length)
{
    /*
     * write LZSS reference token encoded in a big-endian 16-bit value
     */
    if (!dst)
        err_quit("null pointer in writeToken");
    if ((distance >> TOKEN_DISTANCE_BITS) > 0)
        err_quit("invalid reference token distance");
    if ((length >> TOKEN_LENGTH_BITS) > 0)
        err_quit("invalid reference token length");
    uint32_t val = (distance << TOKEN_LENGTH_BITS);
    val |= length;
    bitarray_append(dst, 1); // mark the beginning of a token
    bitarray_appendByte(dst, val >> 8);
    bitarray_appendByte(dst, val & 0xff);
}

int readToken(BitArrayReader *src, unsigned *distance, unsigned *length)
{
    /*
     * read LZSS reference token
     */
    if (!src)
        err_quit("null pointer in writeToken");
    int ret = 0;
    unsigned char temp;
    unsigned val;
    ret += bitarrayreader_readByte(src, &temp);
    val = temp << 8;
    ret += bitarrayreader_readByte(src, &temp);
    val |= temp;
    *distance = val >> TOKEN_LENGTH_BITS;
    *length = val & ((1 << TOKEN_LENGTH_BITS) - 1);
    return ret;
}

Buffer *lzss_extract(Buffer *src)
{
    // turn Buffer into BitArray
    BitArray *compressed = bitarray_fromBuffer(src);
    // generate a reader from the bitarray
    BitArrayReader *reader = bitarray_createReader(compressed);

    size_t decoded_length = bitarrayreader_readInteger(reader);
    Buffer *decompressed = decodeLZSSPayloadBitLevel(reader, decoded_length);

    delete_bitarrayreader(reader);
    delete_bitarrayPreserveContents(compressed);

    return decompressed;
}
