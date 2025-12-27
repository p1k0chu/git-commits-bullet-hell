#include "buffer.h"

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define die(s) fprintf(stderr, "%s\n", s);

char *Buffer_get_line(Buffer *buffer) {
    // remove old line
    size_t len = strnlen(buffer->buffer_start, buffer->buffer_size);
    if (len != buffer->buffer_size) {
        memmove(buffer->buffer_start,
                buffer->buffer_start + len + 1,
                buffer->buffer_size - len - 1);
        memset(buffer->buffer_start + buffer->buffer_size - len - 1, 0, len + 1);
    }

    char   *end_of_line = NULL;
    ssize_t nbytes;
    len = strnlen(buffer->buffer_start, buffer->buffer_size);
    do {
        if (len >= buffer->buffer_size) {
            char *ptr = realloc(buffer->buffer_start, len + 10);
            if (!ptr) die("realloc");

            buffer->buffer_start = ptr;
            buffer->buffer_size  = len + 10;
        }

        nbytes = read(buffer->fd, buffer->buffer_start + len, buffer->buffer_size - len);
        if (nbytes == -1) die("read");

        len += nbytes;
    } while ((end_of_line = (char *)strnchr(buffer->buffer_start, '\n', buffer->buffer_size)) ==
                 NULL &&
             nbytes != 0);

    if (end_of_line == NULL) {
        if (len >= buffer->buffer_size) {
            char *ptr = realloc(buffer->buffer_start, len + 1);
            if (!ptr) die("realloc");

            buffer->buffer_start = ptr;
            buffer->buffer_size  = len + 1;
        }
        buffer->buffer_start[len] = '\0';
    } else {
        *end_of_line = '\0';
    }
    return buffer->buffer_start;
}
