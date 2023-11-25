#include "ealloc.h"
#include "error.h"
#include "stdlib.h"
#include "ringbuffer.h"
#include "ringbuffer_private.h"

RingBuffer *new_ringbuffer(size_t size)
{
    RingBuffer *ret = mmalloc(sizeof(RingBuffer));
    ret->data = mmalloc(sizeof(size));
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

void ringbuffer_set(RingBuffer *dst, char val, ssize_t pos);
void ringbuffer_append(RingBuffer *dst, char val)
{
    if (!dst)
        err_quit("null pointer appending to ring buffer");

    if (dst->len >= dst->size) {
    } else {

    }
}

ssize_t calcIndex(RingBuffer *rb, ssize_t pos)
{
    if (!rb)
        err_quit("null pointer calculating ring buffer index");

    if (pos < 0)
        pos = rb->len+pos;

    if (pos > rb->len || pos < 0)
        err_quit("ring buffer index out of bounds");

    return (rb->size + rb->start + pos) % rb->size;
}
