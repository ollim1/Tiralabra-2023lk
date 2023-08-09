#ifndef BUFFER_H
#define BUFFER_H
#define BUFSIZE 4096
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * The Buffer struct
 * a byte buffer that helps pass binary data around
 */
typedef struct buffer_st {
    size_t len;
    size_t size;
    char *data;
} Buffer;

Buffer *new_buffer();
void delete_buffer(Buffer *);

void buffer_pad(Buffer *buf, size_t len);
void buffer_append(Buffer *, char *, size_t);

#endif
