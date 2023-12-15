#ifndef FILEOPS_H
#define FILEOPS_H
#define READBUF 4096
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#include "buffer.h"
#include "error.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

Buffer *readFile(char *);
void writeFile(Buffer *, char *);
#endif
