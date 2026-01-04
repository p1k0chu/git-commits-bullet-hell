#include "buffer.h"

#include "utils.h"

#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#else
#include <sys/types.h>
#endif

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

        nbytes = SDL_ReadIO(buffer->stream, buffer->buffer_start + len, buffer->buffer_size - len);
        if (SDL_GetIOStatus(buffer->stream) == SDL_IO_STATUS_ERROR) {
            fprintf(stderr, "failed to sdl_readio: %s\n", SDL_GetError());
            exit(EXIT_FAILURE);
        }

        len += nbytes;
    } while ((end_of_line = (char *)strnchr(buffer->buffer_start, '\n', buffer->buffer_size)) ==
                 NULL &&
             SDL_GetIOStatus(buffer->stream) != SDL_IO_STATUS_EOF);

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
