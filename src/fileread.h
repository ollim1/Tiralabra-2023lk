#ifndef FILEREAD_H
#define FILEREAD_H
#define READBUF 4096
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "buffer.h"
#include "error.h"

Buffer *readFile(char *);
void writeFile(Buffer *, char *);
#endif
