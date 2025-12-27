#include <unistd.h>

const char *strnchr(const char *s, int c, size_t n) {
    for (const char *ptr = s; ptr < s + n; ptr++) {
        if (*ptr == c) return ptr;
    }
    return NULL;
}

