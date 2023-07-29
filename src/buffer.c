#include "buffer.h"

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

    size_t new_size = dest->size;
    while (dest->len + len > new_size)
        new_size *= 2;
    if (new_size > dest->size) {
        char *new_data = realloc(dest->data, new_size);
        if (!new_data)
            err_quit("failed to allocate memory for appending to buffer");
        dest->data = new_data;
        dest->size = new_size;
    }
    memcpy(dest->data + dest->len, src, len);
}
