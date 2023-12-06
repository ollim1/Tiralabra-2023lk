#ifndef EALLLOC_H
#define EALLLOC_H

#include <sys/types.h>

void *mmalloc(size_t size);
void *mrealloc(void *ptr, size_t size);
void *mcalloc(size_t nmemb, size_t size);

#endif
