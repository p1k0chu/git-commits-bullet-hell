#pragma once

#include <stddef.h>

#define die(s)                      \
    {                               \
        fprintf(stderr, "%s\n", s); \
        exit(1);                    \
    }
#define sdl_die(s)                  \
    {                               \
        SDL_Log(s, SDL_GetError()); \
        return SDL_APP_FAILURE;     \
    }

#define STR(s)  #s
#define XSTR(s) STR(s)

#define size_of_array(arr) (sizeof(arr) / sizeof(arr[0]))

const char *strnchr(const char *s, int c, size_t n);

