#include "buffer.h"

void buffer_resize(Buffer *buf, size_t new_size);

Buffer *new_buffer()
{
    Buffer *ret = malloc(sizeof(Buffer));
    char *data = malloc(BUFSIZE);
    if (!ret || !data)
        err_quit("failed to allocate memory");
    ret->data = data;
    ret->size = BUFSIZE;
    ret->len = 0;
    return ret;
}

void del_buffer(Buffer *buf)
{
    if (buf) {
        if (buf->data)
            free(buf->data);
        free(buf);
    }
}

void buffer_append(Buffer *dest, char *src, size_t len)
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

    char *newData = realloc(buf->data, newSize);
    if (!newData)
        err_quit("failed to allocate memory for appending to buffer");
    buf->data = newData;

    buf->size = newSize;
}
