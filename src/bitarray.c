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

    int bit = val > 0; // normalize val to 1
    size_t byte = pos / 8;
    size_t offset = pos % 8;
    char destVal = ba->data->data[byte];
    destVal &= ~(1 << offset); // clear bit for writing
    destVal |= (bit << offset); // set bit value
    ba->data->data[byte] = destVal; // write back to Buffer
}

void bitarray_append(BitArray *ba, int val)
{
    if (!ba)
        err_quit("null pointer when appending BitArray bit");
    if (ba->len % 8 == 0) {
        bitarray_pad(ba, 1);
    }
    
    bitarray_set(ba, val, ba->len);
    ba->len += 1;
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
    return (ba->data->data[pos] & (1 << offset)) > 0;
}

void bitarray_appendString(BitArray *dst, char *src, size_t len)
{
    if (!dst)
        err_quit("null pointer when appending bit string to BitArray");
}
void bitarray_pad(BitArray *, size_t);
