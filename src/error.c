#include "error.h"
#include <stdio.h>
#include <stdlib.h>

void err_quit(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}
