#ifndef RINGBUFFER_PRIVATE_H
#define RINGBUFFER_PRIVATE_H
#include "ringbuffer.h"

ssize_t calcRevIndex(RingBuffer *rb, size_t pos);
ssize_t calcIndex(RingBuffer *rb, size_t pos);
#endif
