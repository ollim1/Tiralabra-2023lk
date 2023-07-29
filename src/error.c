#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void err_quit(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}
