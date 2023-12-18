#include "../include/buffer.h"
#include "../include/ealloc.h"

void buffer_resize(Buffer *buf, size_t new_size);

/**
 * Create an empty buffer with a BUFSIZE initial size
 * @return the newly created Buffer
 */
Buffer *new_buffer()
{
    Buffer *ret = mmalloc(sizeof(Buffer));
    unsigned char *data = mcalloc(BUFSIZE, 1);
    ret->data = data;
    ret->size = BUFSIZE;
    ret->len = 0;
    return ret;
}

/**
 * Delete buffer, ignoring any any pointers contained within its elements
 * @param buf the Buffer to delete
 */
void delete_buffer(Buffer *buf)
{
    if (buf) {
        if (buf->data)
            free(buf->data);
        free(buf);
    }
}

/**
 * Check Buffer equality
 * @param a, b buffers to compare
 */
int buffer_equals(Buffer *a, Buffer *b)
{
    if (!a || !b)
        return 0;
    if (a->len != b->len)
        return 0;
    for (size_t i = 0; i < a->len; i++) {
        if (a->data[i] != b->data[i])
            return 0;
    }
    return 1;
}

/**
 * Duplicate a Buffer
 * @param src source Buffer
 */
Buffer *buffer_copy(Buffer *src)
{
    if (!src)
        err_quit("null pointer duplicating buffer");
    return buffer_copyl(src, src->len);
}

/**
 * Duplicate a Buffer up to len bytes.
 * @param src source Buffer
 * @param len length limit of new Buffer
 */
Buffer *buffer_copyl(Buffer *src, size_t len)
{
    if (!src)
        err_quit("null pointer duplicating buffer");

    Buffer *ret = new_buffer();
    buffer_append(ret, src->data, len < src->len ? len : src->len);
    return ret;
}

/**
 * Pad buffer with zero bytes.
 * @param buf the buffer to modify
 * @param len the amount of bytes to append
 */
void buffer_pad(Buffer *buf, size_t len)
{
    if (!buf)
        err_quit("null pointer when resizing buffer");

    size_t newSize = buf->size;
    while (buf->len + len > newSize)
        newSize *= 2;

    if (newSize > buf->size) {
        buffer_resize(buf, newSize);
    }
    memset(buf->data + buf->len, 0, len);
    buf->len += len;
}

/**
 * Truncate Buffer to only the last character. Useful when looking ahead in
 * another data set.
 * @param buf the Buffer
 */
void buffer_truncate(Buffer *buf)
{
    if (!buf)
        err_quit("null pointer truncating buffer");
    if (buf->len < 1)
        return;
    buf->data[0] = buf->data[buf->len - 1];
    buf->len = 1;
}

/**
 * Shrink buffer length by one.
 * Boilerplate for more consistent operation
 * @param buf the Buffer
 */
void buffer_shrink(Buffer *buf)
{

    if (!buf)
        err_quit("null pointer truncating buffer");
    if (buf->len < 1)
        return;
    buf->len--;
}

void buffer_concatl(Buffer *dest, Buffer *src, size_t len)
{
    if (!dest || !src)
        err_quit("null pointer when appending to buffer");

    buffer_append(dest, src->data, len);
}

void buffer_append(Buffer *dest, unsigned char *src, size_t len)
{
    if (!dest || !src)
        err_quit("null pointer when appending to buffer");

    size_t newSize = dest->size;
    while (dest->len + len > newSize)
        newSize *= 2;

    if (newSize > dest->size)
        buffer_resize(dest, newSize);

    memcpy(dest->data + dest->len, src, len);
    dest->len += len;
}

void buffer_resize(Buffer *buf, size_t newSize)
{
    if (!buf)
        err_quit("cannot expand buffer: null pointer");

    unsigned char *newData = mrealloc(buf->data, newSize);
    buf->data = newData;
    buf->size = newSize;
}

void buffer_clear(Buffer *buf)
{
    if (!buf)
        err_quit("null pointer in buffer_clear");
    buf->len = 0;
}

BufferReader *buffer_createReader(Buffer *buffer)
{
    if (!buffer)
        err_quit("null pointer creating BufferReader");

    BufferReader *ret = mmalloc(sizeof(BufferReader));
    ret->data = buffer;
    ret->pos = 0;
    return ret;
}

void delete_bufferreader(BufferReader *reader)
{
    if (reader)
        free(reader);
}

/**
 * Read buffer using BufferReader.
 * @param reader the reader to use
 * @param dst the destination to copy the data to
 * @param len the amount of bytes to read
 * @return number of bytes read, -1 if reader is final
 */
ssize_t bufferreader_read(BufferReader *reader, unsigned char *dst, size_t len)
{
    if (!reader || !dst)
        err_quit("null pointer on bufferreader_read");
    ssize_t total;
    if (reader->pos >= reader->data->len)
        return -1;
    for (total = 0; total < len && reader->pos < reader->data->len; total++, reader->pos++) {
        dst[total] = reader->data->data[reader->pos];
    }
    return total;
}

/**
 * Check if buffer reader has reached the end of the buffer.
 * @param reader the buffer to check
 * @return 1 if true, 0 otherwise
 */
int bufferreader_isFinal(BufferReader *reader)
{
    if (!reader)
        err_quit("null pointer on bufferreader_isFinished");

    return reader->pos >= reader->data->len;
}
