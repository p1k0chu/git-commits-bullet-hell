#pragma once

#include <stddef.h>

typedef struct Buffer {
    char *buffer_start;
    int   fd;

    size_t buffer_size;
} Buffer;

char *Buffer_get_line(Buffer *buffer);

