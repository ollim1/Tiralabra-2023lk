#include "../include/bitarray.h"
#include "../include/ealloc.h"

/**
 * Allocates and initializes a new BitArray.
 * @return the newly created BitArray
 */
BitArray *new_bitarray()
{
    BitArray *ret = mmalloc(sizeof(BitArray));

    Buffer *data = new_buffer();
    ret->data = data;
    ret->len = 0;
    return ret;
}

/**
 * Create BitArray and initialize it with string of ones and zeroes.
 * @param code the code from which to initialized the BitArray
 * @param maxLen maximum length of code
 * @return the newly created BitArray
 */
BitArray *new_bitarray_fromStringl(char *code, size_t maxLen)
{
    BitArray *ret = new_bitarray();
    for (size_t i = 0; i < maxLen && code[i]; i++) {
        if (code[i] != '0' && code[i] != '1')
            err_quit("invalid format while initing BitArray");
        bitarray_append(ret, code[i] == '1');
    }
    return ret;
}

/**
 * Frees memory allocated for BitArray.
 * @params ba the BitArray to delete
 */
void delete_bitarray(BitArray *ba)
{
    if (!ba)
        err_quit("null pointer when deleting BitArray");

    delete_buffer(ba->data);
    free(ba);
}

/**
 * Frees memory allocated for BitArray while preserving contents.
 * Boilerplate for better readability.
 * @param ba the BitArray to delete
 */
void delete_bitarrayPreserveContents(BitArray *ba)
{
    if (!ba)
        err_quit("null pointer when deleting BitArray");

    free(ba);
}

/**
 * Takes the Buffer out of a BitArray and returns it.
 * Deletes the original BitArray.
 * @param ba the BitArray to convert
 * @return the Buffer contained in the BitArray
 */
Buffer *bitarray_deleteAndConvertToBuffer(BitArray *ba)
{
    if (!ba)
        err_quit("null pointer in bitarray_convertToBuffer");

    Buffer *ret = ba->data;
    ret->len = (ba->len - 1) / 8 + 1;
    free(ba);
    return ret;
}

/**
 * Turns a Buffer into a BitArray.
 * @param src buffer to encapsulate
 * @return the newly created BitArray
 */
BitArray *bitarray_fromBuffer(Buffer *src)
{
    if (!src)
        err_quit("null pointer when converting Buffer to BitArray");

    BitArray *ret = mmalloc(sizeof(BitArray));
    ret->data = src;
    ret->len = src->len * 8;
    return ret;
}

/**
 * Copy bits from src to a new BitArray.
 * @param src the source BitArray
 * @param len the desired maximum length of the new BitArray
 * @return the newly created BitArray
 */
BitArray *bitarray_copyl(BitArray *src, size_t len)
{
    if (!src)
        err_quit("null pointer when setting BitArray bit");
    if (len > src->len)
        err_quit("tried to copy more bits that source BitArray contains");

    BitArray *ret = new_bitarray();
    bitarray_concatl(ret, src, len);
    return ret;
}

/**
 * Compare the contents of two BitArrays.
 * @param a the first BitArray
 * @param a the second BitArray
 * @return 1 if BitArrays are the same, 0 if not
 */
int bitarray_equals(BitArray *a, BitArray *b)
{
    if (!a || !b)
        err_quit("null pointer when comparing BitArrays");

    if (a->len != b->len)
        return 0;
    for (size_t i = 0; i < a->len; i++)
        if (bitarray_get(a, i) != bitarray_get(b, i))
            return 0;
    return 1;
}

/**
 * Produce a Buffer from a BitArray by copying the internal buffer. Does not
 * preserve BitArray length.
 * @param src the BitArray from which to extract a Buffer
 * @return the newly created Buffer
 */
Buffer *bitarray_toBuffer(BitArray *src)
{
    if (!src)
        err_quit("null pointer converting BitArray");

    return buffer_copyl(src->data, (src->len - 1) / 8 + 1);
}

/**
 * Convert BitArray to string of signed char. Mostly used for debugging purposes.
 * @param src the source BitArray
 * @return a string with the contents of the BitArray verbatim
 */
char *bitarray_toString(BitArray *src)
{
    if (!src)
        err_quit("null pointer converting BitArray to string");

    char *str = mmalloc(src->len + 1);
    for (size_t i = 0; i < src->len; i++)
        str[i] = bitarray_get(src, i) ? '1' : '0';
    str[src->len] = '\0';
    return str;
}

/**
 * Concatenate BitArrays a and b.
 * @param a the destination BitArray
 * @param b the BitArray to append to a
 */
void bitarray_concat(BitArray *a, BitArray *b)
{
    if (!a || !b)
        err_quit("null pointer when concatenating BitArrays");
    
    bitarray_concatl(a, b, b->len);
}

/**
 * Concatenate BitArrays a and b, with length limit
 * @param a the destination BitArray
 * @param b the BitArray to append to a
 * @param len the maximum amount of bits to append
 */
void bitarray_concatl(BitArray *a, BitArray *b, size_t len)
{
    /*
     * concatenate BitArrays a and b
     */
    if (!a || !b)
        err_quit("null pointer when concatenating BitArrays");
    
    bitarray_appendString(a, b->data->data, (len < b->len ? len : b->len));
}

/**
 * Set BitArray bit.
 * @param dst the BitArray to modify
 * @param val the binary value of the bit
 * @param pos the position of the bit to set
 */
void bitarray_set(BitArray *dst, int val, size_t pos)
{
    if (!dst)
        err_quit("null pointer when setting BitArray bit");
    if (pos >= dst->len)
        err_quit("tried to set bit outside BitArray bounds");

    unsigned char bit = val > 0; // normalize val to 1
    size_t byte = pos / 8;
    size_t offset = pos % 8;

    dst->data->data[byte] &= ~(1 << offset);  // clear bit for writing
    dst->data->data[byte] |= (bit << offset); // set bit value
}

/**
 * Append bit value to BitArray.
 * @param dst the BitArray to modify
 * @param val the binary value of the bit to append
 */
void bitarray_append(BitArray *dst, int val)
{
    if (!dst)
        err_quit("null pointer when appending BitArray bit");
    if (dst->len % 8 == 0)
        buffer_pad(dst->data, 1);

    dst->len++;
    bitarray_set(dst, val, dst->len - 1);
}

/**
 * Append a byte to BitArray.
 * @param dst the BitArray to modify
 * @param byte the byte value to append to dst
 */
void bitarray_appendByte(BitArray *dst, unsigned char byte)
{
    if (!dst)
        err_quit("null pointer when appending bit string to BitArray");

    size_t pos = dst->len;
    bitarray_pad(dst, 8);
    bitarray_setByte(dst, byte, pos);
}

/**
 * Append a string of bits to BitArray.
 * @param dst the BitArray to modify
 * @param src the source string, written as is
 * @param len the length of the string
 */
void bitarray_appendString(BitArray *dst, unsigned char *src, size_t len)
{
    if (!dst)
        err_quit("null pointer when appending bit string to BitArray");

    size_t pos = dst->len;
    bitarray_pad(dst, len);
    bitarray_setString(dst, src, len, pos);
}

/**
 * Set a string of bits in BitArray.
 * @param dst the BitArray to modify
 * @param src the source string, written as is
 * @param len the length of the string
 * @param pos the desired starting position of the string in the destination BitArray
 */
void bitarray_setString(BitArray *dst, unsigned char *src, size_t len, size_t pos)
{
    if (!dst)
        err_quit("null pointer when appending bit string to BitArray");

    size_t byteLimit = len / 8;
    size_t byte = 0;
    // write whole bytes at a time until we run out
    for (; byte < byteLimit; byte++, pos += 8) {
        bitarray_setByte(dst, src[byte], pos);
    }
    // write the remainder one bit at a time
    for (size_t i = byte * 8; i < len; i++) {
        int byte = i / 8;
        int offset = i % 8;
        unsigned char bit = (src[byte] & (1 << offset)) > 0;
        bitarray_set(dst, bit, pos++);
    }
}

/**
 * Set a byte in BitArray.
 * @param dst the BitArray to modify
 * @param value byte to be written
 * @param len the length of the string
 * @param pos the desired starting position of the byte in the destination BitArray
 */
void bitarray_setByte(BitArray *dst, unsigned char value, size_t pos)
{
    if (!dst)
        err_quit("null pointer when setting byte in BitArray");
    if (pos + 8 > dst->len)
        err_quit("BitArray index out of bounds when setting byte");

    int offset = pos % 8;
    int byte = pos / 8;
    if (offset == 0) {
        // if position is byte aligned, just write it out
        dst->data->data[pos / 8] = value;
    } else {
        // if position is not byte aligned, work around the byte boundary using bit masks
        unsigned char mask = (1 << offset) - 1;
        dst->data->data[byte] &= mask;
        dst->data->data[byte] |= (value << offset);
        dst->data->data[byte + 1] &= ~mask;
        dst->data->data[byte + 1] |= (value >> (8 - offset));
    }
}

/**
 * Read a single bit from BitArray.
 * @param src the source BitArray
 * @param pos the position of the bit to retrieve in the BitArray
 * @return the bit value to be returned
 */
int bitarray_get(BitArray *src, size_t pos)
{
    if (!src)
        err_quit("null pointer when appending BitArray bit");
    if (pos >= src->len)
        err_quit("tried to read outside BitArray bounds");

    size_t byte = pos / 8;
    size_t offset = pos % 8;
    return (src->data->data[byte] & (1 << offset)) > 0;
}

/**
 * Read an 8 bit sequence from BitArray
 * @param src the source BitArray
 * @param pos the position of the byte to retrieve in the BitArray
 * @return the byte value to be returned
 */
unsigned char bitarray_getByte(BitArray *src, size_t pos)
{
    if (!src)
        err_quit("null pointer when reading byte from BitArray");
    if (pos + 8 > src->len)
        err_quit("BitArray index out of bounds when reading byte");

    unsigned char value = 0;
    int offset = pos % 8;
    if (offset == 0) {
        value = src->data->data[pos / 8];
    } else {
        int byte1 = src->data->data[pos / 8];
        int byte2 = src->data->data[pos / 8 + 1];
        unsigned char mask = (1 << offset) - 1;
        value = (byte1 & ~mask) >> offset;
        value |= (byte2 & mask) << (8 - offset);
    }

    return value;
}

/**
 * Pad BitArray with zeroes
 * @param arr the array to modify
 * @param len the amount of zeroes to append
 */
void bitarray_pad(BitArray *arr, size_t len)
{
    if (!arr)
        err_quit("null pointer when padding BitArray");

    buffer_pad(arr->data, (len - 1) / 8 + 1);
    arr->len += len;
}

/**
 * Create BitArrayReader using existing BitArraya
 * @param src source BitArray
 * @return the newly created reader
 */
BitArrayReader *bitarray_createReader(BitArray *dst)
{
    BitArrayReader *ret = mmalloc(sizeof(BitArrayReader));
    ret->data = dst;
    ret->pos = 0;
    return ret;
}

/**
 * Iterate over bitarray. Return -1 if position in reader struct is beyond
 * bitarray length; otherwise return number of bits read (should be 1)
 * @param reader the reader to use
 * @param dst pointer to the destination integer
 * @return amount of bits read; one if a bit was read, -1 otherwise
 */
int bitarrayreader_readBit(BitArrayReader *reader, int *dst)
{
    if (!reader || !dst)
        err_quit("null pointer accessing bitarrayreader");
    if (reader->pos >= reader->data->len)
        return -1;
    
    *dst = bitarray_get(reader->data, reader->pos);
    reader->pos++;
    return 1;
}

/**
 * Iterate over bitarray, read 8 bits into an unsigned char.
 * Otherwise return number of bits read (should be 8).
 * @param reader the reader to use
 * @param dst pointer to the destination byte
 * @return amount of bits read; number of bits if read succeeded, -1 otherwise
 */
int bitarrayreader_readByte(BitArrayReader *br, unsigned char *dst)
{
    if (!br || !dst)
        err_quit("null pointer accessing bitarrayreader");
    if (br->pos + 8 > br->data->len)
        return -1;
    
    *dst = bitarray_getByte(br->data, br->pos);
    br->pos += 8;
    return 8;
}

/**
 * Delete BitArrayReader.
 * @param reader the reader to delete
 */
void delete_bitarrayreader(BitArrayReader *reader)
{
    if (!reader)
        return;

    free(reader);
}

/**
 * Encode unsigned integer in bit packed format: 1 bit for end marker, 8 bits
 * for data. Little endian order.
 * @param dst destination BitArray
 */
void bitarray_writeInteger(BitArray *dst, size_t val)
{
    while (val > 0) {
        bitarray_append(dst, 0);
        bitarray_appendByte(dst, val & 0xff);
        val >>= 8;
    }
    bitarray_append(dst, 1); // append end marker
}

/**
 * Decode unsigned integer in bit packed format: 1 bit for end marker, 8 bits
 * for data. Little endian order.
 * @param src source BitArray
 * @return the integer that was read
 */
size_t bitarrayreader_readInteger(BitArrayReader *src)
{
    /*
     * decode integer
     */
    int end = 0;
    
    if (bitarrayreader_readBit(src, &end) != 1)
        err_quit("failed to read bit in decodeLength");

    size_t ret = 0;
    int offset = 0;
    while (!end) {
        unsigned char byte = 0;
        if (bitarrayreader_readByte(src, &byte) != 8)
            err_quit("failed to read byte in decodeLength");
        ret |= (byte << offset);
        offset += 8;
        if (bitarrayreader_readBit(src, &end) != 1)
            err_quit("failed to read bit in decodeLength");
    }
    return ret;
}
