#include "bitarray.h"

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
