#include "ealloc.h"
#include "error.h"
#include "stdlib.h"
#include "ringbuffer.h"
#include "ringbuffer_private.h"

RingBuffer *new_ringbuffer(size_t size)
{
    RingBuffer *ret = mmalloc(sizeof(RingBuffer));
    ret->data = mmalloc(size);
    ret->start = 0;
    ret->end = 0;
    ret->len = 0;
    ret->size = size;
    return ret;
}
void delete_ringbuffer(RingBuffer *rb)
{
    if (!rb || !rb->data)
        err_quit("null pointer in delete_ringbuffer");

    free(rb->data);
    free(rb);
}

char ringbuffer_get(RingBuffer *src, ssize_t pos)
{
    if (!src)
        err_quit("null pointer in ringbuffer_get");

    return src->data[calcIndex(src, pos)];
}

char ringbuffer_getRev(RingBuffer *src, ssize_t pos)
{
    /*
     * get value from buffer, indexing in reverse from the end
     */
    if (!src)
        err_quit("null pointer in ringbuffer_get");

    return src->data[calcRevIndex(src, pos)];
}

void ringbuffer_set(RingBuffer *dst, unsigned char val, ssize_t pos)
{
    if (!dst)
        err_quit("null pointer in ringbuffer_set");
    dst->data[calcIndex(dst, pos)] = val;
}

void ringbuffer_append(RingBuffer *dst, unsigned char val)
{
    if (!dst)
        err_quit("null pointer appending to ring buffer");

    dst->data[dst->end] = val;
    dst->end = (dst->end + 1) % dst->size;
    if (dst->len >= dst->size) {
        dst->start = (dst->start + 1) % dst->size;
        dst->len = dst->size;
    } else {
        dst->len++;
    }
}

void ringbuffer_appendString(RingBuffer *dst, unsigned char *data, size_t len)
{
    for (size_t i = 0; i < len; i++)
        ringbuffer_append(dst, data[i]);
}

ssize_t calcRevIndex(RingBuffer *rb, size_t pos)
{
    if (!rb)
        err_quit("null pointer calculating ring buffer index");

    if (pos >= rb->len)
        err_quit("ring buffer index out of bounds");

    return (rb->start + rb->len - pos - 1) % rb->size;
}

ssize_t calcIndex(RingBuffer *rb, size_t pos)
{
    if (!rb)
        err_quit("null pointer calculating ring buffer index");

    if (pos >= rb->len)
        err_quit("ring buffer index out of bounds");

    return (rb->start + pos) % rb->size;
}
