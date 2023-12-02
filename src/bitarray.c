#include "bitarray.h"
#include "ealloc.h"

BitArray *new_bitarray()
{
    BitArray *ret = mmalloc(sizeof(BitArray));

    Buffer *data = new_buffer();
    ret->data = data;
    ret->len = 0;
    return ret;
}

BitArray *new_bitarray_fromStringl(char *code, size_t maxLen)
{
    /*
     * create BitArray and initialize it with code given as
     * string consisting of ones and zeroes, limit size to maxLen
     */
    BitArray *ret = new_bitarray();
    for (size_t i = 0; i < maxLen && code[i]; i++) {
        if (code[i] != '0' && code[i] != '1')
            err_quit("invalid format while initing BitArray");
        bitarray_append(ret, code[i] == '1');
    }
    return ret;
}

void delete_bitarray(BitArray *ba)
{
    if (!ba)
        err_quit("null pointer when deleting BitArray");

    delete_buffer(ba->data);
    free(ba);
}

BitArray *bitarray_fromBuffer(Buffer *src)
{
    /*
     * turns a Buffer into a BitArray
     */
    if (!src)
        err_quit("null pointer when converting Buffer to BitArray");

    BitArray *ret = mmalloc(sizeof(BitArray));
    ret->data = src;
    ret->len = src->len * 8;
    return ret;
}

BitArray *bitarray_copyl(BitArray *ba, size_t len)
{
    /*
     * copy len bits from ba to a new BitArray
     */
    if (!ba)
        err_quit("null pointer when setting BitArray bit");
    if (len > ba->len)
        err_quit("tried to copy more bits that source BitArray contains");

    BitArray *ret = new_bitarray();
    bitarray_concatl(ret, ba, len);
    return ret;
}

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

Buffer *bitarray_toBuffer(BitArray *ba)
{
    /*
     * return the buffer from a BitArray
     * for now, just return the Buffer object directly
     */
    if (!ba)
        err_quit("null pointer converting BitArray");

    return buffer_copyl(ba->data, (ba->len / 8) + 1);
}

char *bitarray_toString(BitArray *ba)
{
    if (!ba)
        err_quit("null pointer converting BitArray to string");

    char *str = mmalloc(ba->len + 1);
    for (size_t i = 0; i < ba->len; i++)
        str[i] = bitarray_get(ba, i) ? '1' : '0';
    str[ba->len] = '\0';
    return str;
}

void bitarray_concat(BitArray *a, BitArray *b)
{
    /*
     * concatenate BitArrays a and b
     */
    if (!a || !b)
        err_quit("null pointer when concatenating BitArrays");
    
    bitarray_concatl(a, b, b->len);
}

void bitarray_concatl(BitArray *a, BitArray *b, size_t len)
{
    /*
     * concatenate BitArrays a and b
     */
    if (!a || !b)
        err_quit("null pointer when concatenating BitArrays");
    
    bitarray_appendString(a, b->data->data, (len < b->len ? len : b->len));
}

void bitarray_set(BitArray *ba, int val, size_t pos)
{
    /*
     * set BitArray bit at bit position pos
     */
    if (!ba)
        err_quit("null pointer when setting BitArray bit");
    if (pos >= ba->len)
        err_quit("tried to set bit outside BitArray bounds");

    unsigned char bit = val > 0; // normalize val to 1
    size_t byte = pos / 8;
    size_t offset = pos % 8;

    ba->data->data[byte] &= ~(1 << offset);  // clear bit for writing
    ba->data->data[byte] |= (bit << offset); // set bit value
}

void bitarray_append(BitArray *ba, int val)
{
    if (!ba)
        err_quit("null pointer when appending BitArray bit");
    if (ba->len % 8 == 0)
        buffer_pad(ba->data, 1);

    ba->len++;
    bitarray_set(ba, val, ba->len - 1);
}

void bitarray_appendByte(BitArray *dst, unsigned char byte)
{
    /*
     * append a string of bits to BitArray
     */
    if (!dst)
        err_quit("null pointer when appending bit string to BitArray");

    bitarray_appendString(dst, &byte, 8);
}

void bitarray_appendString(BitArray *dst, unsigned char *src, size_t len)
{
    /*
     * append a string of bits to BitArray
     */
    if (!dst)
        err_quit("null pointer when appending bit string to BitArray");

    for (size_t i = 0; i < len; i++) {
        int byte = i / 8;
        int offset = i % 8;
        unsigned char bit = (src[byte] & (1 << offset)) > 0;
        bitarray_append(dst, bit);
    }
}

void bitarray_setString(BitArray *dst, unsigned char *src, size_t len, size_t pos)
{
    /*
     * append a string of bits to BitArray
     */
    if (!dst)
        err_quit("null pointer when appending bit string to BitArray");

    for (size_t i = 0; i < len; i++) {
        int byte = i / 8;
        int offset = i % 8;
        unsigned char bit = (src[byte] & (1 << offset)) > 0;
        bitarray_set(dst, bit, pos++);
    }
}

int bitarray_get(BitArray *ba, size_t pos)
{
    /*
     * read single bit from BitArray
     */
    if (!ba)
        err_quit("null pointer when appending BitArray bit");
    if (pos >= ba->len)
        err_quit("tried to read outside BitArray bounds");

    size_t byte = pos / 8;
    size_t offset = pos % 8;
    return (ba->data->data[byte] & (1 << offset)) > 0;
}

unsigned char bitarray_getByte(BitArray *src, size_t pos)
{
    /*
     * read an 8 bit sequence from BitArray
     */
    if (!src)
        err_quit("null pointer when reading byte from BitArray");
    if (pos + 8 > src->len)
        err_quit("BitArray index out of bounds when reading byte");

    unsigned char ret = 0;
    for (size_t i = 0; i < 8; i++) {
        int bit = bitarray_get(src, pos + i);
        ret |= (bit << i);
    }
    return ret;
}

void bitarray_pad(BitArray *ba, size_t len)
{
    /*
     * pad BitArray by len bits
     */
    if (!ba)
        err_quit("null pointer when padding BitArray");

    buffer_pad(ba->data, len / 8 + 1);
    ba->len += len;
}

BitArrayReader *bitarray_createReader(BitArray *ba)
{
    BitArrayReader *ret = mmalloc(sizeof(BitArrayReader));
    ret->data = ba;
    ret->pos = 0;
    return ret;
}

int bitarrayreader_readBit(BitArrayReader *br, int *dst)
{
    /*
     * iterate over bitarray
     * return -1 if position in reader struct is beyond bitarray length
     * otherwise return number of bits read (should be 1)
     */
    if (!br || !dst)
        err_quit("null pointer accessing bitarrayreader");
    if (br->pos >= br->data->len)
        return -1;
    
    *dst = bitarray_get(br->data, br->pos);
    br->pos++;
    return 1;
}

int bitarrayreader_readByte(BitArrayReader *br, unsigned char *dst)
{
    /*
     * iterate over bitarray, read 8 bits into an unsigned char
     * return -1 if position in reader struct is beyond bitarray length
     * otherwise return number of bits read (should be 8)
     */
    if (!br || !dst)
        err_quit("null pointer accessing bitarrayreader");
    if (br->pos + 8 > br->data->len)
        return -1;
    
    *dst = bitarray_getByte(br->data, br->pos);
    br->pos += 8;
    return 8;
}

void delete_bitarrayreader(BitArrayReader *br)
{
    if (!br)
        return;

    free(br);
}

void bitarray_writeInteger(BitArray *ba, size_t val)
{
    /*
     * encode Huffman buffer length in bit packed format:
     * 1 bit for end marker, 8 bits for data
     * little endian order
     */
    while (val > 0) {
        bitarray_append(ba, 0);
        bitarray_appendByte(ba, val & 0xff);
        val >>= 8;
    }
    bitarray_append(ba, 1); // append end marker
}

size_t bitarrayreader_readInteger(BitArrayReader *br)
{
    /*
     * decode Huffman buffer length
     */
    int end = 0;
    
    if (bitarrayreader_readBit(br, &end) != 1)
        err_quit("failed to read bit in decodeLength");

    size_t ret = 0;
    int offset = 0;
    while (!end) {
        unsigned char byte = 0;
        if (bitarrayreader_readByte(br, &byte) != 8)
            err_quit("failed to read byte in decodeLength");
        ret |= (byte << offset);
        offset += 8;
        if (bitarrayreader_readBit(br, &end) != 1)
            err_quit("failed to read bit in decodeLength");
    }
    return ret;
}
