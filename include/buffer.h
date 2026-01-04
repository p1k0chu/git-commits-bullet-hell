#pragma once

#include <SDL3/SDL_iostream.h>
#include <stddef.h>

typedef struct Buffer {
    char         *buffer_start;
    SDL_IOStream *stream;

    size_t buffer_size;
} Buffer;

char *Buffer_get_line(Buffer *buffer);

