#ifndef RINGBUFFER_H
#define RINGBUFFER_H
#include <sys/types.h>

/*
 * A fixed size ring buffer with automatic rotation of contents, replaces first
 * entries when len hits size
 */

typedef struct ringbuffer_st {
    unsigned char *data;
    size_t start;
    size_t end;
    size_t len;
    size_t size;
} RingBuffer;

RingBuffer *new_ringbuffer(size_t size);
void delete_ringbuffer(RingBuffer *rb);
char ringbuffer_get(RingBuffer *src, ssize_t pos);
void ringbuffer_set(RingBuffer *dst, char val, ssize_t pos);
void ringbuffer_append(RingBuffer *dst, char val);
#endif
