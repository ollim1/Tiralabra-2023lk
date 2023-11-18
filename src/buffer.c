#include "buffer.h"

void buffer_resize(Buffer *buf, size_t new_size);

Buffer *new_buffer()
{
    /*
     * create an empty buffer with a BUFSIZE initial size
     */
    Buffer *ret = malloc(sizeof(Buffer));
    unsigned char *data = malloc(BUFSIZE);
    if (!ret || !data)
        err_quit("failed to allocate memory");
    ret->data = data;
    ret->size = BUFSIZE;
    ret->len = 0;
    return ret;
}

void delete_buffer(Buffer *buf)
{
    /*
     * delete buffer, ignoring any any pointers contained within its elements
     */
    if (buf) {
        if (buf->data)
            free(buf->data);
        free(buf);
    }
}

Buffer *buffer_copy(Buffer *buf)
{
    /*
     * duplicate a Buffer
     */
    if (!buf)
        err_quit("null pointer duplicating buffer");

    Buffer *ret = new_buffer();
    buffer_append(ret, buf->data, buf->len);
    return ret;
}

void buffer_pad(Buffer *buf, size_t len)
{
    /*
     * pad buffer with len zeroes
     */
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

void buffer_append(Buffer *dest, unsigned char *src, size_t len)
{
    if (!dest || !src)
        err_quit("null pointer when appending to buffer");

    size_t newSize = dest->size;
    while (dest->len + len > newSize)
        newSize *= 2;

    if (newSize > dest->size) {
        buffer_resize(dest, newSize);
    }
    memcpy(dest->data + dest->len, src, len);
    dest->len += len;
}

void buffer_resize(Buffer *buf, size_t newSize)
{
    if (!buf)
        err_quit("cannot expand buffer: null pointer");

    unsigned char *newData = realloc(buf->data, newSize);
    if (!newData)
        err_quit("failed to allocate memory for appending to buffer");
    buf->data = newData;

    buf->size = newSize;
}
