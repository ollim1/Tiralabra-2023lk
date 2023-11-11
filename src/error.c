#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "error.h"

void err_quit(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

void err_sys(char *str)
{
    // print system error message and exit with error code
    char *err = strerror(errno);
    int errcode = errno;
    fprintf(stderr, "err_sys %s: %s", str, err);
    exit(errcode);
}
