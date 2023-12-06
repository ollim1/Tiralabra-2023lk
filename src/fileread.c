#include "../include/fileread.h"

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

    unsigned char buf[READBUF];
    size_t len;
    do { // read READBUF bytes at a time
        len = read(fd, buf, READBUF);
        buffer_append(ret, buf, len);
    } while (len == READBUF); // stop when less than READBUF bytes are retrieved
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
        fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd <= 2)
            err_quit("failed to open file for writing");
    }
    fprintf(stderr, "writing %ld bytes\n", src->len);
    unsigned char *ptr = src->data; // use a pointer to move in the buffer
    unsigned char *endptr = src->data + src->len;
    while (ptr < endptr) {
        ssize_t count = MIN(READBUF, endptr - ptr);
        if (write(fd, ptr, count) < count) // progress READBUF bytes at a time
            err_quit("writing to file failed");
        ptr += READBUF;
    }
    if (fd > 2)
        close(fd);
}
