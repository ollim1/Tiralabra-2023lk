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
    unsigned char *data;
} Buffer;
typedef struct bufferReader_st {
    Buffer *data;
    size_t pos;
} BufferReader;

Buffer *new_buffer();
void delete_buffer(Buffer *);

int buffer_equals(Buffer *a, Buffer *b);
Buffer *buffer_copyl(Buffer *buf, size_t len);
void buffer_concatl(Buffer *dest, Buffer *src, size_t len);
void buffer_pad(Buffer *buf, size_t len);
void buffer_truncate(Buffer *buf);
void buffer_append(Buffer *, unsigned char *, size_t);
void buffer_clear(Buffer *buf);
BufferReader *buffer_createReader(Buffer *buffer);
void delete_bufferreader(BufferReader *reader);
ssize_t bufferreader_read(BufferReader *reader, unsigned char *dst, size_t len);
int bufferreader_isFinal(BufferReader *reader);

#endif
