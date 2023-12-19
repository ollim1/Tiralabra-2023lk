#include "../include/lzss_byte.h"
#include "../include/lzss_common.h"
#include "../include/ealloc.h"
#include "../include/error.h"
#include "../include/lzss_byte_private.h"
#include "../include/ringbuffer.h"
#include "../include/buffer.h"
#include <stdint.h>

/**
 * Compresses Buffer using the LZSS (Lempel-Ziv-Storer-Szymanski algorithm.
 * Byte-level variant.
 * @param src input Buffer
 * @return LZSS-compressed output
 */
Buffer *lzss_byte_compress(Buffer *src)
{

    if (!src)
        err_quit("null pointer in huffman_compress");
    if (src->len == 0) {
        fputs("file is empty, skipping compression\n", stderr);
        return src;
    }

    Buffer *compressed = new_buffer();

    encodeLZSSPayloadByteLevel(src, compressed);

    return compressed;
}

/**
 * Decompress LZSS-compressed Buffer.
 * @param src compressed source Buffer
 * @return decompressed Buffer
 */
Buffer *lzss_byte_extract(Buffer *src)
{
    // generate a reader from the buffer
    BufferReader *reader = buffer_createReader(src);

    Buffer *decompressed = decodeLZSSPayloadByteLevel(reader);

    delete_bufferreader(reader);

    return decompressed;
}

/**
 * Encode LZSS payload. Byte-level implementation. Byte literals are output
 * as-is, while tokens are prefixed by a 0xff byte. If the next byte is 0x00,
 * the token should be read as a 0xff literal.
 * @param src the source Buffer
 * @param dst the destination Buffer
 */
void encodeLZSSPayloadByteLevel(Buffer *src, Buffer *dst)
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
        // append to search buffer
        buffer_append(search, &c, 1);
        int searchIndex = findMatchKMP(dictionary, search);

        // if a matching string is in the ring buffer, then check if the
        // previous search string was
        if (searchPrev->len == TOKEN_MAXLEN || searchIndex == -1 || i == src->len - 1) {
            if (i == src->len - 1 && searchIndex != -1) {
                buffer_append(searchPrev, &c, 1);
                searchIndexPrev = searchIndex;
            }

            int distance = searchIndexPrev - searchPrev->len + 1;
            int length = searchPrev->len;
            if (length > 2 && distance > length && distance + length < symbols_output) {
                writeByteToken(dst, distance, length);
            } else {
                writeByteString(dst, searchPrev);
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
        writeByteString(dst, search);

    delete_buffer(searchPrev);
    delete_buffer(search);
    delete_ringbuffer(dictionary);
}

/**
 * Write LZSS reference token encoded in a little-endian 16-bit value.
 * Integer format: dddddddddddd:llll, 12 distance bits:4 length bits
 * On disk: dddd:llll dddddddd
 * @param dst destination BitArray
 * @param distance distance of the end of referenced string from current position
 * @param length length of referenced string
 */
int writeByteToken(Buffer *dst, unsigned distance, unsigned length)
{
    if (!dst)
        err_quit("null pointer in writeToken");
    if ((distance >> TOKEN_DISTANCE_BITS) > 0)
        err_quit("invalid reference token distance");
    if ((length >> TOKEN_LENGTH_BITS) > 0)
        err_quit("invalid reference token length");
    uint32_t val = (distance << TOKEN_LENGTH_BITS);
    val |= length;
    unsigned char temp = 0xff;
    buffer_append(dst, &temp, 1);
    temp = val & 0xff;
    buffer_append(dst, &temp, 1);
    temp = val >> 8;
    buffer_append(dst, &temp, 1);

    return 3; // return number of bytes written
}

/**
 * Write string of LZSS literals to Buffer. 0xff values are escaped using a
 * 0x00 byte afterward.
 * @param dst destination Buffer
 * @param src the source Buffer containing the bytes to write
 * @return amount of literals written
 */
int writeByteString(Buffer *dst, Buffer *src)
{
    if (!dst || !src)
        err_quit("null pointer writing literal");
    size_t i = 0;
    for (i = 0; i < src->len; i++) {
        unsigned char c = src->data[i];
        if (c == 0xff) {
            // check if 0xff indicates token or a 0xff literal
            buffer_append(dst, &c, 1);
            c = 0;
            buffer_append(dst, &c, 1);
        } else
            buffer_append(dst, &c, 1);
    }
    return i;
}

/**
 * Read LZSS reference token. Format is little-endian, so first byte will always
 * be nonzero as it contains all of the length bits.
 * @param reader reader for source Buffer
 * @param distance pointer to distance variable
 * @param length pointer to length variable
 * @return amount of bytes read
 */
int readByteToken(BufferReader *reader, unsigned *distance, unsigned *length)
{
    if (!reader)
        err_quit("null pointer reading byte token");
    unsigned char temp;
    unsigned val = 0;
    int ret = 0;
    if (bufferreader_read(reader, &temp, 1) < 1)
        err_quit("failed to read byte token");
    if (temp == 0) {
        // escaping token into a 0xff literal
        *distance = 0;
        *length = 0;
        return 1;
    }
    val = temp;
    ret += bufferreader_read(reader, &temp, 1);
    val |= temp << 8;
    *distance = val >> TOKEN_LENGTH_BITS;
    *length = val & ((1 << TOKEN_LENGTH_BITS) - 1);

    return ret;
}

/**
 * Decode LZSS payload.
 * @param reader BufferReader for reading input stream.
 * @param decoded_length length of decoded output in bytes
 * @return Buffer containing decoded output
 */
Buffer *decodeLZSSPayloadByteLevel(BufferReader *reader)
{
    if (!reader)
        err_quit("null pointer when decoding LZSS payload");

    // output buffer
    Buffer *output = new_buffer();
    // temp buffer for token expansion
    Buffer *temp = new_buffer();
    while (!bufferreader_isFinal(reader)) {
        unsigned char byte = 0;

        bufferreader_read(reader, &byte, 1);

        if (byte == 0xff) {
            // token byte, the next bytes will either indicate a two byte token or a one byte 0xff
            // literal
            unsigned distance = 0;
            unsigned length = 0;
            if (readByteToken(reader, &distance, &length) < 1) {
                fprintf(stderr, "at byte %lu:", output->len);
                err_quit("failed to read token");
            }
            if (length == 0) {
                // zero byte following 0xff indicates a 0xff literal
                // length is always nonzero in a valid token
                byte = 0xff;
                buffer_append(output, &byte, 1);
            } else {
                // copy string indicated by token
                if (distance > output->len || output->len - distance + length > output->len) {
                    fprintf(stderr, "distance:%5u, length:%3u, file length: %luB\n", distance,
                            length, output->len);
                    err_quit("token string out of bounds");
                }
            }
            // read string referenced by token to temp buffer
            buffer_append(temp, &output->data[output->len - distance], length);
            // append temp buffer contents to output
            buffer_concatl(output, temp, length);
            buffer_clear(temp);
        } else {
            // normal literal
            buffer_append(output, &byte, 1);
        }
    }
    delete_buffer(temp);

    return output;
}
