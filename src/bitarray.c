#include "bitarray.h"
#include "ealloc.h"

BitArray *new_bitarray()
{
    BitArray *ret = malloc(sizeof(BitArray));
    if (!ret)
        err_quit("failed to allocate memory for BitArray");

    Buffer *data = new_buffer();
    ret->data = data;
    ret->len = 0;
    return ret;
}

void delete_bitarray(BitArray *ba)
{
    if (!ba)
        err_quit("null pointer when deleting BitArray");

    delete_buffer(ba->data);
    free(ba);
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

    char bit = val > 0; // normalize val to 1
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

void bitarray_appendByte(BitArray *dst, char byte)
{
    /*
     * append a string of bits to BitArray
     */
    if (!dst)
        err_quit("null pointer when appending bit string to BitArray");

    for (size_t i = 0; i < 8; i++) {
        int byte = i / 8;
        int offset = i % 8;
        char bit = (byte & (1 << offset)) > 0;
        bitarray_append(dst, bit);
    }
}

void bitarray_appendString(BitArray *dst, char *src, size_t len)
{
    /*
     * append a string of bits to BitArray
     */
    if (!dst)
        err_quit("null pointer when appending bit string to BitArray");

    for (size_t i = 0; i < len; i++) {
        int byte = i / 8;
        int offset = i % 8;
        char bit = (src[byte] & (1 << offset)) > 0;
        bitarray_append(dst, bit);
    }
}

void bitarray_setString(BitArray *dst, char *src, size_t len, size_t pos)
{
    /*
     * append a string of bits to BitArray
     */
    if (!dst)
        err_quit("null pointer when appending bit string to BitArray");

    for (size_t i = 0; i < len; i++) {
        int byte = i / 8;
        int offset = i % 8;
        char bit = (src[byte] & (1 << offset)) > 0;
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

int bitarrayreader_readByte(BitArrayReader *br, char *dst)
{
    /*
     * iterate over bitarray, read 8 bits into a char
     * return -1 if position in reader struct is beyond bitarray length
     * otherwise return number of bits read (should be 8)
     */
    if (!br || !dst)
        err_quit("null pointer accessing bitarrayreader");
    if (br->pos + 7 >= br->data->len)
        return -1;
    
    int temp = 0;
    int offset = 0;
    char ret = 0;
    for (size_t i = br->pos; i < 8; i++) {
        bitarrayreader_readBit(br, &temp);
        ret |= (temp << offset);
    }
    *dst = ret;
    return 8;
}

void delete_bitarrayreader(BitArrayReader *br)
{
    if (!br)
        return;

    free(br);
}
