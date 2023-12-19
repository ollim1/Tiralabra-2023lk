#include "../include/lzss.h"
#include "../include/lzss_common.h"
#include "../include/ealloc.h"
#include "../include/error.h"
#include "../include/lzss_private.h"
#include "../include/bitarray.h"
#include "../include/ringbuffer.h"
#include "../include/buffer.h"
#include <stdint.h>

/**
 * Compresses Buffer using the LZSS (Lempel-Ziv-Storer-Szymanski algorithm.
 * Bit-level variant.
 * @param src input Buffer
 * @return LZSS-compressed output
 */
Buffer *lzss_compress(Buffer *src)
{

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
    Buffer *ret = bitarray_deleteAndConvertToBuffer(compressed);

    return ret;
}

/**
 * Encode LZSS payload. Bit-level implementation. Each byte literal is prefixed
 * by a 'zero' bit, while two-byte tokens are prefixed by 'one' bits.
 * @param src the source Buffer
 * @param dst the destination BitArray
 */
void encodeLZSSPayloadBitLevel(Buffer *src, BitArray *dst)
{
    RingBuffer *dictionary = new_ringbuffer(WINDOW_SIZE);
    Buffer *search = new_buffer();
    Buffer *searchPrev = new_buffer();
    int searchIndexPrev = -1;
    size_t i;
    size_t symbols_output = 0;

    for (i = 0; i < src->len; i++) {
        // read byte from input
        unsigned char c = src->data[i];
        // append to search buffer and look up current lookahead string in the
        // ring buffer
        buffer_append(search, &c, 1);
        int searchIndex = findMatchKMP(dictionary, search);

        // If string match fails, check if previous match succeeded.
        if (searchPrev->len == TOKEN_MAXLEN || searchIndex == -1 || i == src->len - 1) {
            if (i == src->len - 1 && searchIndex != -1) {
                buffer_append(searchPrev, &c, 1);
                searchIndexPrev = searchIndex;
            }

            // calculate token information
            int distance = searchIndexPrev - searchPrev->len + 1;
            int length = searchPrev->len;
            if (length > 1 && distance > length && distance + length < symbols_output) {
                // skip encoding token if the amount of bytes represented is
                // smaller than the length of the token itself
                writeToken(dst, distance, length);
            } else {
                writeString(dst, searchPrev);
            }
            // add up the amount of amount of bytes written
            symbols_output += searchPrev->len;
            // clear search buffers when done
            buffer_clear(searchPrev);
            buffer_truncate(search);
        }
        // store current search result for next iteration
        searchIndexPrev = searchIndex;
        buffer_append(searchPrev, &c, 1);
        // append to dictionary buffer
        ringbuffer_append(dictionary, c);
    }
    // if some bytes are still left in the search buffer, append them to output
    if (symbols_output < i && search->len > 0)
        writeString(dst, search);

    delete_buffer(searchPrev);
    delete_buffer(search);
    delete_ringbuffer(dictionary);
}

/**
 * Decode LZSS payload.
 * @param reader BitArrayReader for reading input stream.
 * @param decoded_length length of decoded output in bytes
 * @return Buffer containing decoded output
 */
Buffer *decodeLZSSPayloadBitLevel(BitArrayReader *reader, size_t decoded_length)
{
    if (!reader)
        err_quit("null pointer when decoding LZSS payload");

    // output and temp buffers
    Buffer *output = new_buffer();
    Buffer *temp = new_buffer();
    while (output->len < decoded_length) {
        int bit = 0;

        // read a bit to check if next byte is a literal or part of a token
        if (bitarrayreader_readBit(reader, &bit) < 1)
            err_quit("unexpected end of file while reading payload bit");

        if (bit == 1) {
            // token bit set, the next two bytes will contain a token
            unsigned distance = 0;
            unsigned length = 0;
            if (readToken(reader, &distance, &length) < 16)
                err_quit("unexpected end of file while reading payload token");
            // copy string indicated by token
            if (distance > output->len || output->len - distance + length > output->len) {
                fprintf(stderr, "distance:%5u, length:%3u, file length: %lu\n", distance, length, output->len);
                err_quit("token string out of bounds");
            }
            buffer_append(temp, &output->data[output->len - distance], length);
            buffer_concatl(output, temp, length);
            buffer_clear(temp);
        } else {
            // token bit unset, next byte will be a literal
            unsigned char byte = 0;
            if (bitarrayreader_readByte(reader, &byte) < 8)
                err_quit("unexpected end of file while reading payload literal");
            buffer_append(output, &byte, 1);
        }
    }
    delete_buffer(temp);

    return output;
}

/**
 * Write string of LZSS literals to BitArray.
 * @param dst destination BitArray
 * @param src the source Buffer containing the bytes to write
 */
void writeString(BitArray *dst, Buffer *src)
{
    if (!dst || !src)
        err_quit("null pointer writing literal");
    for (size_t i = 0; i < src->len; i++) {
        bitarray_append(dst, 0);
        bitarray_appendByte(dst, src->data[i]);
    }
}

/**
 * Write LZSS reference token encoded in a big-endian 16-bit value.
 * Integer format: dddddddddddd:llll, 12 distance bits:4 length bits
 * @param dst destination BitArray
 * @param distance distance of the end of referenced string from current position
 * @param length length of referenced string
 */
void writeToken(BitArray *dst, unsigned distance, unsigned length)
{
    if (!dst)
        err_quit("null pointer in writeToken");
    if ((distance >> TOKEN_DISTANCE_BITS) > 0) {
        fprintf(stderr, "distance:%5u, length:%3u\n", distance, length);
        err_quit("invalid reference token distance");
    }
    if ((length >> TOKEN_LENGTH_BITS) > 0) {
        fprintf(stderr, "distance:%5u, length:%3u\n", distance, length);
        err_quit("invalid reference token length");
    }
    if (distance < length) {
        fprintf(stderr, "distance:%5u, length:%3u\n", distance, length);
        err_quit("invalid reference token distance");
    }
    uint32_t val = (distance << TOKEN_LENGTH_BITS);
    val |= length;
    bitarray_append(dst, 1); // mark the beginning of a token
    bitarray_appendByte(dst, val >> 8);
    bitarray_appendByte(dst, val & 0xff);
}

/**
 * Read LZSS reference token.
 * @param reader reader for source BitArray 
 * @param distance pointer to distance variable
 * @param length pointer to length variable
 * @return amount of bits read
 */
int readToken(BitArrayReader *reader, unsigned *distance, unsigned *length)
{
    if (!reader)
        err_quit("null pointer in writeToken");
    int ret = 0;
    unsigned char temp;
    unsigned val;
    ret += bitarrayreader_readByte(reader, &temp);
    val = temp << 8;
    ret += bitarrayreader_readByte(reader, &temp);
    val |= temp;
    *distance = val >> TOKEN_LENGTH_BITS;
    *length = val & ((1 << TOKEN_LENGTH_BITS) - 1);
    return ret;
}

/**
 * Decompress LZSS-compressed Buffer.
 * @param src compressed source Buffer
 * @return decompressed Buffer
 */
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
