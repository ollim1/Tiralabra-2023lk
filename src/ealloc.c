#include <stdlib.h>
#include "../include/error.h"

void *mmalloc(size_t size)
{
    // wrapper for malloc
    void *ret = malloc(size);
    if (!ret)
        err_sys("malloc");
    return ret;
}

void *mrealloc(void *ptr, size_t size)
{
    // wrapper for realloc
    void *ret = realloc(ptr, size);
    if (!ret)
        err_sys("realloc");
    return ret;
}

void *mcalloc(size_t nmemb, size_t size)
{
    // wrapper for calloc
    void *ret = calloc(nmemb, size);
    if (!ret)
        err_sys("calloc");
    return ret;
}
