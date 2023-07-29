#include "fileread.h"

Buffer *readFile(char *filename)
{
    /*
     * create buffer and read file specified by filename to buffer
     */
    Buffer *ret = new_buffer();
    int fd = 0; // setting stdin as default file descriptor
    if (strcmp(filename, "-") != 0) {
        fd = open(filename, O_RDONLY);
        if (fd <= 2)
            err_quit("failed to open file for reading");
    }
    char buf[READBUF];
    size_t len;
    do {
        len = read(fd, buf, READBUF);
        buffer_append(ret, buf, len);
    } while (len > READBUF);
    if (fd > 2)
        close(fd);

    return ret;
}

void writeFile(Buffer *src, char *filename)
{
    /*
     * write buffer src to file specified by filename
     */
    int fd = 1; // setting stdout as default file descriptor
    if (strcmp(filename, "-") != 0) {
        fd = open(filename, O_WRONLY);
        if (fd <= 2)
            err_quit("failed to open file for writing");
    }
    ssize_t i;
    for (i = 0; i < src->len; i+= READBUF) {
        int to_write = MIN(READBUF, src->len - i - 1);
        if (write(fd, src->data + i, to_write) < to_write)
            err_quit("writing to file failed");
    }
    if (fd > 2)
        close(fd);
}

