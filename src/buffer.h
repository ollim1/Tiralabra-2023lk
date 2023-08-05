#ifndef BUFFER_H
#define BUFFER_H
#define BUFSIZE 4096
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct buffer_st {
    size_t len;
    size_t size;
    char *data;
} Buffer;

Buffer *new_buffer();
void del_buffer(Buffer *);

void buffer_append(Buffer *, char *, size_t);

#endif
